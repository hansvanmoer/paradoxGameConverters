/*Copyright (c) 2015 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "HoI3Country.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include <io.h>
#include <fstream>
#include <sstream>
#include "../Log.h"
#include "../Configuration.h"
#include "../Parsers/Parser.h"
//#include "HoI3Army.h"
#include "HoI3Relations.h"
#include "../V2World/V2Relations.h"
#include "../V2World/V2Party.h"



const int MONEYFACTOR = 30;	// ducat to pound conversion rate


HoI3Country::HoI3Country(string _tag, string _commonCountryFile, HoI3World* _theWorld, bool _newCountry /* = false */)
{
	theWorld = _theWorld;
	newCountry = _newCountry;

	commonCountryFile = string(removeAccented(_commonCountryFile));

	tag					= _tag;

	provinces.clear();

	capital			= 0;
	diploPoints		= 0.0;
	badboy			= 0.0;
	money				= 0.0;
	government		= "";
	faction = "";

	neutrality = 50;
	national_unity = 70;

	training_laws = "minimal_training";
	press_laws = "censored_press";
	industrial_policy_laws = "consumer_product_orientation";
	educational_investment_law = "minimal_education_investment";
	economic_law = "full_civilian_economy";
	conscription_law = "volunteer_army";
	civil_law = "limited_restrictions";

	relations.clear();
	armies.clear();

	srcCountry	= NULL;
}


void HoI3Country::output() const
{
	// Afghanistan example
	//capital = 9191
	//government = absolute_monarchy
	//ideology = paternal_autocrat
	//head_of_state = 54001
	//head_of_government = 54002
	//foreign_minister = 54003
	//armament_minister = 54004
	//minister_of_security = 54002
	//minister_of_intelligence = 54006
	//chief_of_staff = 54007
	//chief_of_army = 54007
	//chief_of_navy = 54001
	//chief_of_air = 54007
	//alignment = { x = 60 y = 78.9 }
	//neutrality = 90
	//national_unity = 50

	//infantry_theory = 0
	//infantry_practical = 0
	//militia_theory = 2.5
	//militia_practical = 2.5
	//mobile_theory = 1
	//mobile_practical = 1
	//artillery_theory = 0
	//artillery_practical = 0
	//rocket_science = 0
	//rocket_practical = 0
	//naval_engineering = 0
	//destroyer_practical = 0
	//cruiser_practical = 0
	//capitalship_practical = 0
	//carrier_practical = 0
	//submarine_engineering = 0
	//submarine_practical = 0
	//electornicegineering_theory = 0
	//electornicegineering_practical = 0
	//automotive_theory = 0
	//armour_practical = 0
	//aeronautic_engineering = 0
	//single_engine_aircraft_practical = 0
	//twin_engine_aircraft_practical = 0
	//four_engine_aircraft_practical = 0
	//spearhead_theory = 0
	//superior_firepower_theory = 0
	//grand_battleplan_theory = 1
	//human_wave_theory = 0
	//land_doctrine_practical = 0
	//naval_doctrine_practical = 0
	//base_strike_doctrine = 0
	//fleet_in_being_doctrine = 0
	//sealane_interdiction_doctrine = 0
	//air_doctrine_practical = 0
	//fighter_focus = 0
	//cas_focus = 0
	//tac_focus = 0
	//nav_focus = 0
	//strategic_air_focus = 0
	//mechanicalengineering_theory = 0
	//chemical_engineering = 0
	//jetengine_theory = 0
	//jetengine_practical = 0
	//nuclear_physics = 0
	//nuclear_bomb = 0
	//transport_practical = 0
	//officers_ratio = 0.60

	//militia_smallarms = 1
	//militia_support = 1
	//militia_guns = 1
	//militia_at = 1
	//infantry_activation = 1
	//schwerpunkt = 0
	//blitzkrieg = 0
	//mobile_warfare = 0
	//elastic_defence = 0
	//operational_level_command_structure = 0
	//tactical_command_structure = 0
	//mechanized_offensive = 0
	//delay_doctrine = 0
	//integrated_support_doctrine = 0
	//infantry_warfare = 0
	//special_forces = 0
	//assault_concentration = 0
	//central_planning = 0
	//mass_assault = 0
	//peoples_army = 1
	//large_front = 0
	//guerilla_warfare = 1
	//operational_level_organisation = 0

	//oob = "AFG_1936.txt"

	//popularity = {
	//	national_socialist = 0
	//	fascistic = 0
	//	paternal_autocrat = 60
	//	social_conservative = 20
	//	market_liberal = 0
	//	social_liberal = 0
	//	social_democrat = 5
	//	left_wing_radical = 10
	//	leninist = 5
	//	stalinist = 0
	//}

	//organization = {
	//	national_socialist = 0
	//	fascistic = 0
	//	paternal_autocrat = 70
	//	social_conservative = 30
	//	market_liberal = 0
	//	social_liberal = 10
	//	social_democrat = 20
	//	left_wing_radical = 10
	//	leninist = 0
	//	stalinist = 0
	//}

	//training_laws = minimal_training
	//press_laws = censored_press
	//industrial_policy_laws = consumer_product_orientation
	//education_investment_law = minimal_education_investment
	//economic_law = full_civilian_economy
	//conscription_law = volunteer_army
	//civil_law = limited_restrictions

	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\countries\\" + filename).c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create country history file " << filename;
		exit(-1);
	}

	if (capital > 0)
	{
		fprintf(output, "capital=%d\n", capital);
	}
	if (government != "")
	{
		fprintf(output, "government = %s\n", government.c_str());
	}
	if (ideology != "")
	{
		fprintf(output, "ideology = %s\n", ideology.c_str());
	}
	if (faction != "")
	{
		fprintf(output, "join_faction = %s\n", faction.c_str());
	}
	fprintf(output, "alignment = { x = %f y = %f }\n", alignment.get2DX(), alignment.get2DY());
	fprintf(output, "neutrality = %i\n", neutrality);
	fprintf(output, "national_unity = %i\n", national_unity);

	fprintf(output, "civil_law = %s\n", civil_law.c_str());
	fprintf(output, "conscription_law = %s\n", conscription_law.c_str());
	fprintf(output, "economic_law = %s\n", economic_law.c_str());
	fprintf(output, "education_investment_law = %s\n", educational_investment_law.c_str());
	fprintf(output, "industrial_policy_laws = %s\n", industrial_policy_laws.c_str());
	fprintf(output, "press_laws = %s\n", press_laws.c_str());
	fprintf(output, "training_laws = %s\n", training_laws.c_str());
	outputPracticals(output);
	outputTech(output);
	outputParties(output);

	fprintf(output, "oob = \"%s\"\n", (tag + "_OOB.txt").c_str());

	fclose(output);

	outputOOB();

	if (newCountry)
	{
		// Output common country file. 
		std::ofstream commonCountryOutput("Output\\" + Configuration::getOutputName() + "\\common\\countries\\" + commonCountryFile);
		commonCountryOutput << "graphical_culture = Generic\n";	// default to "Generic"
		commonCountryOutput << "color = { " << color << " }\n";
		commonCountryOutput << "\n";
		commonCountryOutput << "default_templates = {\n";
		commonCountryOutput << "	generic_infantry = {\n";
		commonCountryOutput << "		infantry_brigade\n";
		commonCountryOutput << "		infantry_brigade\n";
		commonCountryOutput << "		infantry_brigade\n";
		commonCountryOutput << "	}\n";
		commonCountryOutput << "	generic_milita = {\n";
		commonCountryOutput << "		militia_brigade\n";
		commonCountryOutput << "		militia_brigade\n";
		commonCountryOutput << "		militia_brigade\n";
		commonCountryOutput << "	}\n";
		commonCountryOutput << "	generic_armoured = {\n";
		commonCountryOutput << "		armor_brigade\n";
		commonCountryOutput << "		motorized_brigade\n";
		commonCountryOutput << "		motorized_brigade\n";
		commonCountryOutput << "	}\n";
		commonCountryOutput << "	generic_cavalry = {\n";
		commonCountryOutput << "		cavalry_brigade\n";
		commonCountryOutput << "		cavalry_brigade\n";
		commonCountryOutput << "	}\n";
		commonCountryOutput << "}\n";
		commonCountryOutput << "\n";
		commonCountryOutput << "unit_names = {\n";
		//commonCountryOutput << "	infantry_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Division\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	cavalry_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Cavalry\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	motorized_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Motor Div.\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	mechanized_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Half Track Div.\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	light_armor_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Armoured Div.\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	armor_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Armoured Div.\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	paratrooper_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Para Division\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	marine_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Marine Division\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	bergsjaeger_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Mountain Division\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	garrison_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Division\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	hq_brigade = {\n";
		//commonCountryOutput << "		\"1st Afghanestani Army\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	militia_brigade = {\n";
		//commonCountryOutput << "		\"Faizabad Militia\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	multi_role = {\n";
		//commonCountryOutput << "		\"I.Fighter Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	interceptor = {\n";
		//commonCountryOutput << "		\"I.Fighter Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	strategic_bomber = {\n";
		//commonCountryOutput << "		\"I.Strategic Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	tactical_bomber = {\n";
		//commonCountryOutput << "		\"I.Tactical Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	naval_bomber = {\n";
		//commonCountryOutput << "		\"I.Naval Bomber Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	cas = {\n";
		//commonCountryOutput << "		\"I.Dive Bomber Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	transport_plane = {\n";
		//commonCountryOutput << "		\"I.Air Transport Group\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	battleship = {\n";
		//commonCountryOutput << "		\"RAS Afghanistan\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	heavy_cruiser = {\n";
		//commonCountryOutput << "		\"RAS Faizabad\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	destroyer = {\n";
		//commonCountryOutput << "		\"D1 / D2 / D3\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	carrier = {\n";
		//commonCountryOutput << "		\"RAS Zahir Shah\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	submarine = {\n";
		//commonCountryOutput << "		\"1. Submarine Flotilla\"\n";
		//commonCountryOutput << "	}\n";
		//commonCountryOutput << "	transport_ship = {\n";
		//commonCountryOutput << "		\"1. Troop Transport Flotilla\"\n";
		//commonCountryOutput << "	}\n";
		commonCountryOutput << "}\n";
		commonCountryOutput << "\n";
		commonCountryOutput << "ministers = {\n";
		commonCountryOutput << "}\n";
	}
}


void HoI3Country::outputToCommonCountriesFile(FILE* output) const
{
	fprintf(output, "%s = \"countries%s\"\n", tag.c_str(), commonCountryFile.c_str());
}


void HoI3Country::outputPracticals(FILE* output) const
{
	fprintf(output, "\n");
	for (map<string, double>::const_iterator itr = practicals.begin(); itr != practicals.end(); ++itr)
	{
		if (itr->second > 0.0)
		{
			fprintf(output, "%s = %f\n", itr->first.c_str(), min(20.0, itr->second));
		}
	}
}


void HoI3Country::outputTech(FILE* output) const
{
	fprintf(output, "\n");
	for (map<string, int>::const_iterator itr = technologies.begin(); itr != technologies.end(); ++itr)
	{
		fprintf(output, "%s = %d\n", itr->first.c_str(), itr->second);
	}
}


void HoI3Country::outputElection(FILE* output) const
{
	date electionDate = date("1936.1.1");

	if (electionDate.month == 12)
	{
		electionDate.month = 1;
		electionDate.year++;
	}
	else
	{
		electionDate.month++;
	}
	electionDate.year -= 4;
	fprintf(output, "	last_election=%s\n", electionDate.toString().c_str());
}


void HoI3Country::outputParties(FILE* output) const
{
	fprintf(output, "popularity = {\n");
	for (auto party: parties)
	{
		fprintf(output, "\t%s = %d\n", party.idealogy.c_str(), party.popularity);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");

	fprintf(output, "organization = {\n");
	for (auto party: parties)
	{
		fprintf(output, "\t%s = %d\n", party.idealogy.c_str(), party.organization);
	}
	fprintf(output, "}\n");
	fprintf(output, "\n");
}


void HoI3Country::outputOOB() const
{
	FILE* output;
	if (fopen_s(&output, ("Output\\" + Configuration::getOutputName() + "\\history\\units\\" + tag + "_OOB.txt").c_str(), "w") != 0)
	{
		LOG(LogLevel::Error) << "Could not create OOB file " << (tag + "_OOB.txt");
		exit(-1);
	}

	for (std::vector<HoI3RegGroup>::const_iterator armyItr = armies.begin(); armyItr != armies.end(); ++armyItr)
	{
		if (armyItr->getProductionQueue())
		{
			armyItr->outputIntoProductionQueue(output, tag);
		}
		else
		{
			armyItr->output(output);
		}
	}

	fclose(output);
}


void HoI3Country::initFromV2Country(const V2World& _srcWorld, const V2Country* _srcCountry, const string _ideology, vector<string> outputOrder, const CountryMapping& countryMap, governmentMapping governmentMap, inverseProvinceMapping inverseProvinceMap, map<int, int>& leaderMap)
{
	srcCountry = _srcCountry;
	ideology = _ideology;

	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename = fileData.name;
	}
	_findclose(fileListing);
	if (filename == "")
	{
		string filesearch = Configuration::getHoI3Path() + "\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename = fileData.name;
		}
		_findclose(fileListing);
	}
	if (filename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
	}

	// Color
	color = srcCountry->getColor();

	// Localisation
	localisation.SetTag(tag);
	localisation.ReadFromCountry(*srcCountry);

	// Government
	string srcGovernment = srcCountry->getGovernment();
	if (srcGovernment.size() > 0)
	{
		government = getGovernmentForCountry(srcCountry, _ideology, governmentMap);
		if (government.empty())
		{
			government = "";
			LOG(LogLevel::Warning) << "No government mapping defined for " << srcGovernment << " (" << srcCountry->getTag() << " -> " << tag << ')';
		}
	}

	// Political parties
	convertParties(_srcCountry, _srcWorld.getActiveParties(_srcCountry));

	// Faction is handled in HoI3World::configureFactions

	// civil law - democracies get open society, communist dicatorships get totalitarian, everyone else gets limited restrictions
	if (srcGovernment == "democracy" || srcGovernment == "hms_government")
		civil_law = "open_society";
	else if (srcGovernment == "proletarian_dictatorship")
		civil_law = "totalitarian_system";
	else
		civil_law = "limited_restrictions";

	// conscription law - everyone starts with volunteer armies
	conscription_law = "volunteer_army";

	// economic law - everyone starts with full civilian economy
	economic_law = "full_civilian_economy";

	// educational investment law - from educational spending
	if (srcCountry->getEducationSpending() > 0.90)
		educational_investment_law = "big_education_investment";
	else if (srcCountry->getEducationSpending() > 0.70)
		educational_investment_law = "medium_large_education_investment";
	else if (srcCountry->getEducationSpending() > 0.40)
		educational_investment_law = "average_education_investment";
	else
		educational_investment_law = "minimal_education_investment";

	// industrial policy laws - everyone starts with consumer product orientation
	industrial_policy_laws = "consumer_product_orientation";

	// press laws - set from press reforms
	if (srcCountry->getReform("press_rights") == "free_press")
		press_laws = "free_press";
	else if (srcCountry->getReform("press_rights") == "censored_press")
		press_laws = "censored_press";
	else // press_rights == state_press
	{
		if ((srcGovernment == "proletarian_dictatorship") ||
			(srcGovernment == "fascist_dictatorship"))
		{
			press_laws = "propaganda_press";
		}
		else
		{
			press_laws = "state_press";
		}
	}

	// training laws - from military spending
	if (srcCountry->getMilitarySpending() > 0.90)
		training_laws = "specialist_training";
	else if (srcCountry->getMilitarySpending() > 0.70)
		training_laws = "advanced_training";
	else if (srcCountry->getMilitarySpending() > 0.40)
		training_laws = "basic_training";
	else
		training_laws = "minimal_training";

	// Relations
	map<string, V2Relations*> srcRelations = srcCountry->getRelations();
	if (srcRelations.size() > 0)
	{
		for (map<string, V2Relations*>::iterator itr = srcRelations.begin(); itr != srcRelations.end(); ++itr)
		{
			const std::string& HoI3Tag = countryMap[itr->second->getTag()];
			if (!HoI3Tag.empty())
			{
				HoI3Relations* hoi2r = new HoI3Relations(HoI3Tag, itr->second);
				relations.insert(make_pair(HoI3Tag, hoi2r));
			}
		}
	}

	// Capital
	int oldCapital = srcCountry->getCapital();
	inverseProvinceMapping::iterator itr = inverseProvinceMap.find(oldCapital);
	if (itr != inverseProvinceMap.end())
	{
		capital = itr->second[0];
	}
}


// used only for countries which are NOT converted (i.e. unions, dead countries, etc)
void HoI3Country::initFromHistory()
{
	string fullFilename;
	struct _finddata_t	fileData;
	intptr_t					fileListing;
	string filesearch = ".\\blankMod\\output\\history\\countries\\" + tag + "*.txt";
	if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
	{
		filename			= fileData.name;
		fullFilename	= string(".\\blankMod\\output\\history\\countries\\") + fileData.name;
	}
	_findclose(fileListing);
	if (fullFilename == "")
	{
		string filesearch = Configuration::getHoI3Path() + "\\history\\countries\\" + tag + "*.txt";
		if ((fileListing = _findfirst(filesearch.c_str(), &fileData)) != -1L)
		{
			filename			= fileData.name;
			fullFilename = Configuration::getHoI3Path() + "\\history\\countries\\" + fileData.name;
		}
		_findclose(fileListing);
	}
	if (fullFilename == "")
	{
		string countryName	= commonCountryFile;
		int lastSlash			= countryName.find_last_of("/");
		countryName				= countryName.substr(lastSlash + 1, countryName.size());
		filename					= tag + " - " + countryName;
		return;
	}

	Object* obj = doParseFile(fullFilename.c_str());
	if (obj == NULL)
	{
		LOG(LogLevel::Error) << "Could not parse file " << fullFilename;
		exit(-1);
	}

	vector<Object*> results = obj->getValue("government");
	if (results.size() > 0)
	{
		government = results[0]->getLeaf();
	}

	results = obj->getValue("capital");
	if (results.size() > 0)
	{
		capital = atoi(results[0]->getLeaf().c_str());
	}
}


void HoI3Country::convertArmies(const map<int,int>& leaderIDMap, const inverseProvinceMapping& inverseProvinceMap, map<int, V2Province*> allProvinces, vector<int> port_whitelist)
{
}

void HoI3Country::addRelation(HoI3Relations* newRelation)
{
	relations.insert(make_pair(newRelation->getTag(), newRelation));
}

const std::map<string, HoI3Relations*> &HoI3Country::getRelations() const
{
	return relations;
}

HoI3Relations* HoI3Country::getRelations(string withWhom) const
{
	map<string, HoI3Relations*>::const_iterator i = relations.find(withWhom);
	if (i != relations.end())
	{
		return i->second;
	}
	else
	{
		return NULL;
	}
}

vector<int> HoI3Country::getPortProvinces(vector<int> locationCandidates, map<int, HoI3Province*> allProvinces)
{
	// hack for naval bases.  not ALL naval bases are in port provinces, and if you spawn a navy at a naval base in
	// a non-port province, Vicky crashes....
	static vector<int> port_blacklist;
	if (port_blacklist.size() == 0)
	{
		int temp = 0;
		ifstream s("port_blacklist.txt");
		while (s.good() && !s.eof())
		{
			s >> temp;
			port_blacklist.push_back(temp);
		}
		s.close();
	}

	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		vector<int>::iterator black = std::find(port_blacklist.begin(), port_blacklist.end(), *litr);
		if (black != port_blacklist.end())
		{
			locationCandidates.erase(litr);
			break;
		}
	}
	for (vector<int>::iterator litr = locationCandidates.begin(); litr != locationCandidates.end(); ++litr)
	{
		map<int, HoI3Province*>::iterator pitr = allProvinces.find(*litr);
		if (pitr != allProvinces.end())
		{
			if (!pitr->second->isCoastal())
			{
				locationCandidates.erase(litr);
				--pitr;
				break;
			}
		}
	}

	return locationCandidates;
}

#pragma optimize("", off)
void HoI3Country::convertParties(const V2Country* srcCountry, vector<V2Party*> V2Parties)
{
	// sort Vic2 parties by idealogy
	map<string, vector<V2Party*>> V2Idealogies;
	for (auto partyItr: V2Parties)
	{
		string idealogy = partyItr->ideology;
		auto idealogyItr = V2Idealogies.find(idealogy);
		if (idealogyItr == V2Idealogies.end())
		{
			vector<V2Party*> newPartyVector;
			newPartyVector.push_back(partyItr);
			V2Idealogies.insert(make_pair(idealogy, newPartyVector));
		}
		else
		{
			idealogyItr->second.push_back(partyItr);
		}
	}

	// prep unmapped HoI3 parties
	map<string, string> unmappedParties;
	unmappedParties.insert(make_pair("national_socialist", "national_socialist"));
	unmappedParties.insert(make_pair("fascistic", "fascistic"));
	unmappedParties.insert(make_pair("paternal_autocrat", "paternal_autocrat"));
	unmappedParties.insert(make_pair("social_conservative", "social_conservative"));
	unmappedParties.insert(make_pair("market_liberal", "market_liberal"));
	unmappedParties.insert(make_pair("social_liberal", "social_liberal"));
	unmappedParties.insert(make_pair("social_democrat", "social_democrat"));
	unmappedParties.insert(make_pair("left_wing_radical", "left_wing_radical"));
	unmappedParties.insert(make_pair("leninist", "leninist"));
	unmappedParties.insert(make_pair("stalinist", "stalinist"));

	// map all the simplistic cases
	auto idealogyItr = V2Idealogies.find("fascist");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "fascistic";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("fascist") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("fascistic");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("reactionary");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "paternal_autocrat";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("reactionary") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("paternal_autocrat");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("conservative");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "social_conservative";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("conservative") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("social_conservative");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("socialist");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "left_wing_radical";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("socialist") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("left_wing_radical");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("communist");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "stalinist";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("communist") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("stalinist");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("liberal");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "social_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("liberal") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("social_liberal");
		unmappedParties.erase(itr);
	}
	idealogyItr = V2Idealogies.find("anarcho_liberal");
	if ((idealogyItr != V2Idealogies.end()) && (idealogyItr->second.size() == 1))
	{
		HoI3Party newParty;
		newParty.name				= idealogyItr->second[0]->name;
		newParty.idealogy			= "market_liberal";
		newParty.popularity		= static_cast<unsigned int>(srcCountry->getUpperHousePercentage("anarcho_liberal") * 100);
		newParty.organization	= newParty.popularity;
		parties.push_back(newParty);

		V2Idealogies.erase(idealogyItr);
		auto itr = unmappedParties.find("market_liberal");
		unmappedParties.erase(itr);
	}

	if (V2Idealogies.size() > 0)
	{
		LOG(LogLevel::Warning) << "Unmapped Vic2 parties!";
	}
	// map the simple excess cases

	// merge Vic2 parties as needed, then map those cases
}
#pragma optimize("", on)

void HoI3Country::outputLocalisation(FILE* output) const
{
	std::ostringstream localisationStream;
	localisation.WriteToStream(localisationStream);
	std::string localisationString = localisationStream.str();
	fwrite(localisationString.c_str(), sizeof(std::string::value_type), localisationString.size(), output);
}

string HoI3Country::removeAccented(string str) {
	for (unsigned i = 0; i < str.length(); ++i)
	{
		const char*
			//   "����������������������������������������������������������������"
			tr = "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";
		unsigned char ch = (str[i]);
		if (ch >= 192) {
			str[i] = tr[ch - 192];
		}
		// http://stackoverflow.com/questions/14094621/
	}
	return str;
}

void HoI3Country::setTechnology(string tech, int level)
{
	// don't allow downgrades
	map<string, int>::iterator techEntry = technologies.find(tech);
	if (techEntry == technologies.end() || technologies[tech] < level)
		technologies[tech] = level;
}

void HoI3Country::addArmy(HoI3RegGroup _army)
{
	armies.push_back(_army);
}
