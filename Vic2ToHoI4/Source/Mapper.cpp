/*Copyright (c) 2016 The Paradox Game Converters Project

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



#include "Mapper.h"
#include "Log.h"
#include "Configuration.h"
#include "Object.h"
#include "V2World/V2World.h"
#include "V2World/V2Country.h"
#include "V2World/V2Province.h"
#include <algorithm>
#include <sys/stat.h>



void initProvinceMap(Object* obj, provinceMapping& provinceMap, provinceMapping& inverseProvinceMap, resettableMap& resettableProvinces)
{
	vector<Object*> leaves = obj->getLeaves();	// the different version number blocks

	if (leaves.size() < 1)
	{
		LOG(LogLevel::Error) << "No province mapping definitions loaded";
		return;
	}

	vector<Object*> data = leaves[0]->getLeaves();	// the actual mappings
	for (auto mapping: data)
	{
		vector<int> V2nums;					// the V2 province numbers in this mappping
		vector<int> HoI4nums;				// the HoI4 province numbers in this mappping
		bool			resettable = false;	// if this is a province that can be reset to V2 defaults

		vector<Object*> vMaps = mapping->getLeaves();	// the items within the mapping
		for (auto item: vMaps)
		{
			if (item->getKey() == "vic2")
			{
				V2nums.push_back(atoi(item->getLeaf().c_str()));
			}
			else if (item->getKey() == "hoi4")
			{
				HoI4nums.push_back(atoi(item->getLeaf().c_str()));
			}
			else if (item->getKey() == "resettable")
			{
				resettable = true;
			}
			else
			{
				LOG(LogLevel::Warning) << "Unknown data while mapping provinces";
			}
		}

		if (V2nums.size() == 0)
		{
			V2nums.push_back(0);
		}
		if (HoI4nums.size() == 0)
		{
			HoI4nums.push_back(0);
		}

		for (auto num: HoI4nums)
		{
			if (num != 0)
			{
				provinceMap.insert(make_pair(num, V2nums));
				if (resettable)
				{
					resettableProvinces.insert(num);
				}
			}
		}
		for (auto num: V2nums)
		{
			if (num != 0)
			{
				inverseProvinceMap.insert(make_pair(num, HoI4nums));
			}
		}
	}
}


static const vector<int> empty_vec;	// an empty vector in case there are no equivalent V2 province numbers
vector<int> getHoI4ProvinceNums(inverseProvinceMapping invProvMap, const int v2ProvinceNum)
{
	inverseProvinceMapping::iterator itr = invProvMap.find(v2ProvinceNum);	// the province entry in the inverse province map
	if (itr == invProvMap.end())
	{
		return empty_vec;
	}
	else
	{
		return itr->second;
	}
}


HoI4AdjacencyMapping initHoI4AdjacencyMap()
{
	//FILE* adjacenciesBin = NULL;	// the adjacencies.bin file
	//string filename = Configuration::getHoI4Path() + "/tfh/map/cache/adjacencies.bin";
	//fopen_s(&adjacenciesBin, filename.c_str(), "rb");
	//if (adjacenciesBin == NULL)
	//{
	//	LOG(LogLevel::Error) << "Could not open " << filename;
	//	exit(1);
	//}

	HoI4AdjacencyMapping adjacencyMap;	// the adjacency mapping
	//while (!feof(adjacenciesBin))
	//{
	//	int numAdjacencies;	// the total number of adjacencies
	//	if (fread(&numAdjacencies, sizeof(numAdjacencies), 1, adjacenciesBin) != 1)
	//	{
	//		break;
	//	}
	//	vector<adjacency> adjacencies;	// the adjacencies for the current province
	//	for (int i = 0; i < numAdjacencies; i++)
	//	{
	//		adjacency newAdjacency;	// the current adjacency
	//		fread(&newAdjacency, sizeof(newAdjacency), 1, adjacenciesBin);
	//		adjacencies.push_back(newAdjacency);
	//	}
	//	adjacencyMap.push_back(adjacencies);
	//}
	//fclose(adjacenciesBin);

	// optional code to output data from the adjacencies map
	/*FILE* adjacenciesData;
	fopen_s(&adjacenciesData, "adjacenciesData.csv", "w");
	fprintf(adjacenciesData, "From,Type,To,Via,Unknown1,Unknown2\n");
	for (unsigned int from = 0; from < adjacencyMap.size(); from++)
	{
		vector<adjacency> adjacencies = adjacencyMap[from];
		for (unsigned int i = 0; i < adjacencies.size(); i++)
		{
			fprintf(adjacenciesData, "%d,%d,%d,%d,%d,%d\n", from, adjacencies[i].type, adjacencies[i].to, adjacencies[i].via, adjacencies[i].unknown1, adjacencies[i].unknown2);
		}
	}
	fclose(adjacenciesData);*/

	return adjacencyMap;
}


void initContinentMap(Object* obj, continentMapping& continentMap)
{
	continentMap.clear();
	vector<Object*> continentObjs = obj->getLeaves();	// the continents
	for (auto continentObj: continentObjs)
	{
		string continent = continentObj->getKey();	// the current continent
		vector<Object*> provinceObjs = continentObj->getValue("provinces");	// the province numbers in this continent
		for (auto provinceStr: provinceObjs[0]->getTokens())
		{
			const int province = atoi(provinceStr.c_str());	// the current province num
			continentMap.insert( make_pair(province, continent) );
		}
	}
}


void mergeNations(V2World& world, Object* mergeObj)
{
	vector<Object*> rules = mergeObj->getValue("merge_nations");	// all merging rules
	if (rules.size() < 0)
	{
		LOG(LogLevel::Debug) << "No nations have merging requested (skipping)";
		return;
	}

	rules = rules[0]->getLeaves();	// the rules themselves
	for (auto rule: rules)
	{
		vector<Object*> thisMerge = rule->getLeaves();	// the current merge rule
		string masterTag;										// the nation to merge into
		vector<string> slaveTags;								// the nations that will be merged into the master
		bool enabled = false;									// whether or not this rule is enabled
		for (auto item: thisMerge)
		{
			if (item->getKey() == "merge" && item->getLeaf() == "yes")
			{
				enabled = true;
			}
			else if (item->getKey() == "master")
			{
				masterTag = item->getLeaf();
			}
			else if (item->getKey() == "slave")
			{
				slaveTags.push_back(item->getLeaf());
			}
		}
		V2Country* master = world.getCountry(masterTag);	// the actual master country
		if ( enabled && (master != NULL) && (slaveTags.size() > 0) )
		{
			for (auto slave: slaveTags)
			{
				master->eatCountry(world.getCountry(slave));
			}
		}
	}
}

void removeEmptyNations(V2World& world)
{
	map<string, V2Country*> countries = world.getCountries();	// all V2 countries
	for (auto country: countries)
	{
		map<int, V2Province*> provinces	= country.second->getProvinces();	// the provinces for the nation
		vector<V2Province*> cores			= country.second->getCores();			// the cores for the nation
		if ( (provinces.size()) == 0 && (cores.size() == 0) )
		{
			world.removeCountry(country.first);
		}
	}
}


void initStateMap(Object* obj, stateMapping& stateMap, stateIndexMapping& stateIndexMap)
{
	vector<Object*> leaves = obj->getLeaves();	// the states

	for (unsigned int stateIndex = 0; stateIndex < leaves.size(); stateIndex++)
	{
		vector<string>	provinces = leaves[stateIndex]->getTokens();	// the provinces in this state
		vector<int>			neighbors;												// the neighboring provinces (that is, all provinces in the state)

		for (auto provNum: provinces)
		{
			neighbors.push_back( atoi(provNum.c_str()) );
			stateIndexMap.insert(make_pair(atoi(provNum.c_str()), stateIndex));
		}
		for (auto neighbor: neighbors)
		{
			stateMap.insert(make_pair(neighbor, neighbors));
		}
	}
}


unionMapping initUnionMap(Object* obj)
{
	unionMapping unionMap;	// the cultural unions map

	vector<Object*> unions = obj->getLeaves();	// the rules for cultural unions
	for (auto aUnion: unions)
	{
		string tag;		// the tag for the cultural union
		string culture;	// the culture for the cultural union

		for (auto item: aUnion->getLeaves())
		{
			if (item->getKey() == "tag")
			{
				tag = item->getLeaf();
			}
			if (item->getKey() == "culture")
			{
				culture = item->getLeaf();
			}
		}

		unionMap.push_back(make_pair(culture, tag));
	}

	return unionMap;
}


void initUnionCultures(Object* obj, unionCulturesMap& unionCultures)
{
	vector<Object*> cultureGroups = obj->getLeaves();	// the cultural group rules
	for (auto cultureGroup: cultureGroups)
	{
		vector<Object*>		culturesObj		= cultureGroup->getLeaves();	// the items in this rule
		string					group				= cultureGroup->getKey();		// the cultural group
		vector<string>		cultures;												// the cultures

		for (auto item: culturesObj)
		{
			if (item->getKey() == "dynasty_names")
			{
				continue;
			}
			else if (item->getKey() == "graphical_culture")
			{
				continue;
			}
			else
			{
				cultures.push_back(item->getKey());
			}
		}

		unionCulturesMap::iterator itr = unionCultures.find(group);
		if (itr != unionCultures.end())
		{
			vector<string> oldCultures = itr->second;	// any cultures already in the group
			for (auto jtr: oldCultures)
			{
				cultures.push_back(jtr);
			}
		}
		unionCultures[group] = cultures;
	}
}


cultureMapping initCultureMap(Object* obj)
{
	cultureMapping cultureMap;						// the culture mapping
	vector<Object*> rules = obj->getLeaves();	// the culture mapping rules

	for (auto rule: rules)
	{
		vector<Object*>	cultures	= rule->getLeaves();	// the items in this rule
		string				dstCulture;							// the HoI4 culture
		vector<string>	srcCultures;						// the Vic2 cultures

		for (auto item: cultures)
		{
			if (item->getKey() == "HoI4")
			{
				dstCulture = item->getLeaf();
			}
			if (item->getKey() == "v2")
			{
				srcCultures.push_back(item->getLeaf());
			}
		}

		for (auto srcCulture: srcCultures)
		{
			cultureMap.insert(make_pair(srcCulture, dstCulture));
		}
	}

	return cultureMap;
}


void initIdeaEffects(Object* obj, map<string, int>& armyInvIdeas, map<string, int>& commerceInvIdeas, map<string, int>& cultureInvIdeas, map<string, int>& industryInvIdeas, map<string, int>& navyInvIdeas, map<string, double>& UHLiberalIdeas, map<string, double>& UHReactionaryIdeas, vector< pair<string, int> >& literacyIdeas, map<string, int>& orderIdeas, map<string, int>& libertyIdeas, map<string, int>& equalityIdeas)
{
	vector<Object*> ideasObj = obj->getLeaves();
	for (auto ideasItr: ideasObj)
	{
		string idea = ideasItr->getKey();
		vector<Object*> effects = ideasItr->getLeaves();
		for (auto effectsItr: effects)
		{
			string effectType = effectsItr->getKey();
			if (effectType == "army_investment")
			{
				armyInvIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "commerce_investment")
			{
				commerceInvIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "culture_investment")
			{
				cultureInvIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "industry_investment")
			{
				industryInvIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "navy_investment")
			{
				navyInvIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "upper_house_liberal")
			{
				UHLiberalIdeas[idea] = atof(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "upper_house_reactionary")
			{
				UHReactionaryIdeas[idea] = atof(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "NV_order")
			{
				orderIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "NV_liberty")
			{
				libertyIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "NV_equality")
			{
				equalityIdeas[idea] = atoi(effectsItr[0].getLeaf().c_str());
			}
			else if (effectType == "literacy")
			{
				vector<string> literacyStrs = effectsItr[0].getTokens();
				for (auto literacyString: literacyStrs)
				{
					literacyIdeas.push_back(make_pair(idea, atoi(literacyString.c_str())));
				}
			}
		}
	}
}


void initGovernmentJobTypes(Object* obj, governmentJobsMap& governmentJobs)
{
	vector<Object*> jobsObj = obj->getLeaves();
	for (auto jobsItr: jobsObj)
	{
		string job = jobsItr->getKey();
		vector<string> traits;
		auto traitsObj = jobsItr->getLeaves();
		for (auto trait: traitsObj)
		{
			traits.push_back(trait->getLeaf());
		}
		governmentJobs.insert(make_pair(job, traits));
	}
}


void initLeaderTraitsMap(Object* obj, leaderTraitsMap& leaderTraits)
{
	vector<Object*> typesObj = obj->getLeaves();
	for (auto typeItr: typesObj)
	{
		string type = typeItr->getKey();
		vector<string> traits;
		auto traitsObj = typeItr->getLeaves();
		for (auto trait: traitsObj)
		{
			traits.push_back(trait->getLeaf());
		}
		leaderTraits.insert(make_pair(type, traits));
	}
}


void initLeaderPersonalityMap(Object* obj, personalityMap& landPersonalityMap, personalityMap& seaPersonalityMap)
{
	vector<Object*> personalitiesObj = obj->getLeaves();
	for (auto personalityItr: personalitiesObj)
	{
		string personality = personalityItr->getKey();
		vector<string> landTraits;
		vector<string> seaTraits;
		auto traitsObj = personalityItr->getLeaves();
		for (auto trait: traitsObj)
		{
			if (trait->getKey() == "land")
			{
				landTraits.push_back(trait->getLeaf());
			}
			else if (trait->getKey() == "sea")
			{
				seaTraits.push_back(trait->getLeaf());
			}
		}
		landPersonalityMap.insert(make_pair(personality, landTraits));
		seaPersonalityMap.insert(make_pair(personality, seaTraits));
	}
}


void initLeaderBackgroundMap(Object* obj, backgroundMap& landBackgroundMap, backgroundMap& seaBackgroundMap)
{
	vector<Object*> backgroundObj = obj->getLeaves();
	for (auto backgroundItr: backgroundObj)
	{
		string background = backgroundItr->getKey();
		vector<string> landTraits;
		vector<string> seaTraits;
		auto traitsObj = backgroundItr->getLeaves();
		for (auto trait: traitsObj)
		{
			if (trait->getKey() == "land")
			{
				landTraits.push_back(trait->getLeaf());
			}
			else if (trait->getKey() == "sea")
			{
				seaTraits.push_back(trait->getLeaf());
			}
		}
		landBackgroundMap.insert(make_pair(background, landTraits));
		seaBackgroundMap.insert(make_pair(background, seaTraits));
	}
}


void initNamesMapping(Object* obj, namesMapping& namesMap)
{
	vector<Object*> groupsObj = obj->getLeaves();
	for (auto groupsItr: groupsObj)
	{
		vector<Object*> culturesObj = groupsItr->getLeaves();
		for (auto culturesItr: culturesObj)
		{
			string key = culturesItr->getKey();
			if ((key == "union") || (key == "leader") || (key == "unit") || (key == "is_overseas"))
			{
				continue;
			}
			vector<Object*>	firstNamesObj	= culturesItr->getValue("first_names");
			vector<Object*>	lastNamesObj	= culturesItr->getValue("last_names");
			if ((firstNamesObj.size() > 0) && (lastNamesObj.size() > 0))
			{
				vector<string>		firstNames		= firstNamesObj[0]->getTokens();
				vector<string>		lastNames		= lastNamesObj[0]->getTokens();
				namesMap.insert(make_pair(key, make_pair(firstNames, lastNames)));
			}
			else
			{
				LOG(LogLevel::Error) << "No names for " << key;
			}	
		}
	}
}


void initPortraitMapping(Object* obj, portraitMapping& portraitMap)
{
	vector<Object*> groupsObj = obj->getLeaves();
	for (auto groupsItr: groupsObj)
	{
		vector<string> portraits = groupsItr->getTokens();
		portraitMap.insert(make_pair(groupsItr->getKey(), portraits));
	}
}


void initAIFocusModifiers(Object* obj, AIFocusModifiers& modifiers)
{
	vector<Object*> focusesObj = obj->getLeaves();
	for (auto focusesItr: focusesObj)
	{
		pair<AIFocusType, vector<AIFocusModifier>> newFocus;
		string focusName = focusesItr->getKey();
		if (focusName == "sea_focus")
		{
			newFocus.first = SEA_FOCUS;
		}
		else if (focusName == "tank_focus")
		{
			newFocus.first = TANK_FOCUS;
		}
		else if (focusName == "air_focus")
		{
			newFocus.first = AIR_FOCUS;
		}
		else if (focusName == "inf_focus")
		{
			newFocus.first = INF_FOCUS;
		}

		vector<Object*> modifiersObj = focusesItr->getLeaves();
		for (auto modifiersItr: modifiersObj)
		{
			AIFocusModifier newModifier;

			vector<Object*> modifierItems = modifiersItr->getLeaves();

			if (modifierItems.size() > 0)
			{
				string factorStr = modifierItems[0]->getLeaf();
				newModifier.modifierAmount = atof(factorStr.c_str());
			}
			if (modifierItems.size() > 1)
			{
				newModifier.modifierType			= modifierItems[1]->getKey();
				newModifier.modifierRequirement	= modifierItems[1]->getLeaf();
			}

			newFocus.second.push_back(newModifier);
		}

		modifiers.insert(newFocus);
	}
}