#include "Culture.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

CK3::Culture::Culture(std::istream &theStream, long long theID) : ID(theID) {
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void CK3::Culture::registerKeys() {
	registerKeyword("culture_template",
					[this](std::istream &theStream) { culture_template = commonItems::getString(theStream); });
	registerKeyword("name", [this](std::istream &theStream) { localizedName = commonItems::getString(theStream); });
	registerKeyword("heritage", [this](std::istream &theStream) { heritage = commonItems::getString(theStream); });
	registerKeyword("ethos",
					[this](std::istream &theStream) { ethos = commonItems::singleString(theStream).getString(); });
	registerKeyword("traditions", [this](std::istream &theStream) { traditions = commonItems::getStrings(theStream); });
	registerKeyword("name_list",
					[this](std::istream &theStream) {
						auto temp = commonItems::getString(theStream);
						if (temp.size() > 10) {
							temp = temp.substr(10, temp.size()); // drop "name_list_", leave "polish"
							nameLists.insert(temp);
						}
					});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void CK3::Culture::concoctCultureName(const mappers::LocalizationMapper &localizationMapper,
									  std::map<std::string, int> &       cultureNamingCounter) {
	if (culture_template) {
		name = *culture_template;
		return;
	}

	if (!localizedName) {
		name = "noname";
		return;
	}

	const auto &match = localizationMapper.reverseLookupCultureName(*localizedName);
	if (match) {
		auto strippedName = *match;
		strippedName      = strippedName.substr(0, strippedName.size() - 5);
		// In a pure CK3 context, we can just use the stripped name.
		name = strippedName;
		return;
	}

	// Now everything else, we need to Concoct the culture name, finally.
	name = "dynamic-";
	for (const auto &entry: nameLists) {
		// Since we have no EU4 mapping, we just use the CK3 name list name.
		name += entry + "-";
	}
	name += "culture";

	if (cultureNamingCounter.contains(name)) {
		++cultureNamingCounter.at(name);
		name += "-num" + std::to_string(cultureNamingCounter.at(name));
	} else {
		cultureNamingCounter.emplace(name, 1);
		name += "-num1";
	}
}
