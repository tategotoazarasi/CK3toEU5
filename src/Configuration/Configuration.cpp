#include "Configuration.h"
#include "Color.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "GameVersion.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>
auto laFabricaDeColor = commonItems::Color::Factory();

Configuration::Configuration(const commonItems::ConverterVersion &converterVersion) {
	Log(LogLevel::Info) << "Reading configuration file";
	registerKeys();
	parseFile("configuration.txt");
	clearRegisteredKeywords();
	setOutputName();
	verifyCK3Path();
	verifyCK3Version(converterVersion);
	// We don't have EU5, so we can't verify paths or versions.
	// verifyEU5Path();
	// verifyEU5Version(converterVersion);
	Log(LogLevel::Progress) << "3 %";
}

Configuration::Configuration(std::istream &theStream) {
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	setOutputName();
}

void Configuration::registerKeys() {
	registerKeyword("SaveGame",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString path(theStream);
						SaveGamePath = path.getString();
						Log(LogLevel::Info) << "Save Game set to: " << SaveGamePath.string();
					});
	registerKeyword("CK3directory",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString path(theStream);
						CK3Path = path.getString();
					});
	registerKeyword("EU5directory",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString path(theStream);
						EU5Path = path.getString();
					});
	registerKeyword("CK3DocDirectory",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString path(theStream);
						CK3DocPath = path.getString();
					});
	registerKeyword("output_name",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString nameStr(theStream);
						outputName = nameStr.getString();
						Log(LogLevel::Info) << "Output name set to: " << outputName.string();
					});
	registerKeyword("start_date",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString startDateString(theStream);
						startDate = STARTDATE(std::stoi(startDateString.getString()));
						Log(LogLevel::Info) << "Start date set to: " << startDateString.getString();
					});
	registerKeyword("i_am_hre",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString hreString(theStream);
						iAmHre = I_AM_HRE(std::stoi(hreString.getString()));
						Log(LogLevel::Info) << "HRE set to: " << hreString.getString();
					});
	registerKeyword("dejure",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString dejureString(theStream);
						dejure = DEJURE(std::stoi(dejureString.getString()));
						Log(LogLevel::Info) << "DeJure set to: " << dejureString.getString();
					});
	registerKeyword("split_vassals",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString splitVassalsString(theStream);
						splitVassals = SPLITVASSALS(std::stoi(splitVassalsString.getString()));
						Log(LogLevel::Info) << "Split Vassals set to: " << splitVassalsString.getString();
					});
	registerKeyword("shatter_empires",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString shatterEmpiresString(theStream);
						shatterEmpires = SHATTER_EMPIRES(std::stoi(shatterEmpiresString.getString()));
						Log(LogLevel::Info) << "Shatter Empires set to: " << shatterEmpiresString.getString();
					});
	registerKeyword("shatter_level",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString shatterLevelString(theStream);
						shatterLevel = SHATTER_LEVEL(std::stoi(shatterLevelString.getString()));
						Log(LogLevel::Info) << "Shatter Level set to: " << shatterLevelString.getString();
					});
	registerKeyword("shatter_hre_level",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString shatterHRELevelString(theStream);
						shatterHRELevel = SHATTER_HRE_LEVEL(std::stoi(shatterHRELevelString.getString()));
						Log(LogLevel::Info) << "Shatter HRE Level set to: " << shatterHRELevelString.getString();
					});
	registerKeyword("siberia",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString siberiaString(theStream);
						siberia = SIBERIA(std::stoi(siberiaString.getString()));
						Log(LogLevel::Info) << "Siberia set to: " << siberiaString.getString();
					});
	registerKeyword("sunset",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString sunsetString(theStream);
						sunset = SUNSET(std::stoi(sunsetString.getString()));
						Log(LogLevel::Info) << "Sunset set to: " << sunsetString.getString();
					});
	registerKeyword("dynamicInstitutions",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString dynamicInstitutionsString(theStream);
						dynamicInstitutions = INSTITUTIONS(std::stoi(dynamicInstitutionsString.getString()));
						Log(LogLevel::Info) << "Dynamic Institutions set to: " << dynamicInstitutionsString.getString();
					});
	registerKeyword("development",
					[this](const std::string &unused, std::istream &theStream) {
						const commonItems::singleString developmentString(theStream);
						development = DEVELOPMENT(std::stoi(developmentString.getString()));
						Log(LogLevel::Info) << "Development set to: " << developmentString.getString();
					});
	registerKeyword("multiprovdevtransfer",
					[this](std::istream &theStream) {
						const commonItems::singleString multiprovdevtransferString(theStream);
						multiProvinceDevelopmentTransfer = MULTIPROVDEVTRANSFER(
							std::stoi(multiprovdevtransferString.getString()));
						Log(LogLevel::Info) << "Multiple Province Dev Transfer set to: " << multiprovdevtransferString.
								getString();
					});
	registerKeyword("dynasticNames",
					[this](std::istream &theStream) {
						const commonItems::singleString dynasticNamesString(theStream);
						dynasticNames = DYNASTICNAMES(std::stoi(dynasticNamesString.getString()));
						Log(LogLevel::Info) << "Dynastic Names set to: " << dynasticNamesString.getString();
					});
	registerKeyword("discoveredBy",
					[this](std::istream &theStream) {
						const commonItems::singleString discoveredByString(theStream);
						discoveredBy = DISCOVEREDBY(std::stoi(discoveredByString.getString()));
						Log(LogLevel::Info) << "Discovered By set to: " << discoveredByString.getString();
					});
	registerKeyword("tribalConfederations",
					[this](std::istream &theStream) {
						const commonItems::singleString tribalConfederationsString(theStream);
						tribalConfederations = TRIBALCONFEDERATIONS(std::stoi(tribalConfederationsString.getString()));
						Log(LogLevel::Info) << "Tribal Confederations set to: " << tribalConfederationsString.
								getString();
					});
	registerKeyword("celestial",
					[this](std::istream &theStream) {
						const commonItems::singleString celestialString(theStream);
						celestial = CELESTIAL(std::stoi(celestialString.getString()));
						Log(LogLevel::Info) << "Celestial set to: " << celestialString.getString();
					});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void Configuration::verifyCK3Path() {
	if (!commonItems::DoesFolderExist(CK3Path)) throw std::runtime_error(CK3Path.string() + " does not exist!");
	// TODO: OSX and Linux paths are speculative
	if (!commonItems::DoesFileExist(CK3Path / "binaries/ck3.exe") && !commonItems::DoesFileExist(CK3Path / "CK3game") &&
		!commonItems::DoesFileExist(CK3Path / "binaries/ck3"))
		throw std::runtime_error(CK3Path.string() + " does not contain Crusader Kings 3!");
	if (!commonItems::DoesFileExist(CK3Path / "game/map_data/positions.txt")) throw std::runtime_error(
		CK3Path.string() + " does not appear to be a valid CK3 install!");
	Log(LogLevel::Info) << "\tCK3 install path is " << CK3Path.string();
	CK3Path += "/game/"; // We're adding "/game/" since all we ever need from now on is in that subdirectory.
}

void Configuration::verifyEU5Path() const {
	if (!commonItems::DoesFolderExist(EU5Path)) throw std::runtime_error(EU5Path.string() + " does not exist!");
	// Add checks for eu5.exe etc. when the game is out.
	Log(LogLevel::Info) << "\tEU5 install path is " << EU5Path.string();
}

void Configuration::setOutputName() {
	if (outputName.empty()) { outputName = SaveGamePath.filename(); }
	auto outputNameString = outputName.stem().string();
	outputNameString      = replaceCharacter(outputNameString, '-');
	outputNameString      = replaceCharacter(outputNameString, ' ');

	outputName = commonItems::normalizeUTF8Path(outputNameString);
	Log(LogLevel::Info) << "Using output name " << outputName.string();
}

void Configuration::verifyCK3Version(const commonItems::ConverterVersion &converterVersion) const {
	const auto CK3Version = GameVersion::extractVersionFromLauncher(CK3Path / "../launcher/launcher-settings.json");
	if (!CK3Version) {
		Log(LogLevel::Error) << "CK3 version could not be determined, proceeding blind!";
		return;
	}

	Log(LogLevel::Info) << "CK3 version: " << CK3Version->toShortString();

	if (converterVersion.getMinSource() > *CK3Version) {
		Log(LogLevel::Error) << "CK3 version is v" << CK3Version->toShortString() << ", converter requires minimum v"
				<< converterVersion.getMinSource().toShortString() << "!";
		throw std::runtime_error("Converter vs CK3 installation mismatch!");
	}
	if (!converterVersion.getMaxSource().isLargerishThan(*CK3Version)) {
		Log(LogLevel::Error) << "CK3 version is v" << CK3Version->toShortString() << ", converter requires maximum v"
				<< converterVersion.getMaxSource().toShortString() << "!";
		throw std::runtime_error("Converter vs CK3 installation mismatch!");
	}
}

void Configuration::verifyEU5Version(const commonItems::ConverterVersion &converterVersion) const {
	// To be implemented when EU5 is out.
}
