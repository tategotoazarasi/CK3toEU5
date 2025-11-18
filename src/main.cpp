#include "Log.h"
#include "Parser.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

class ConverterConfig: public commonItems::parser
{
public:
	ConverterConfig()
	{
		registerKeyword("configuration", [this](std::istream& theStream) { parseStream(theStream); });
		registerKeyword("save_game",
				[this](std::istream& theStream) {
					saveGamePath = commonItems::getString(theStream);
					Log(LogLevel::Info) << "Target Save Game: " << saveGamePath;
				});
		registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
	}

	void load(const std::string& filename) { parseFile(filename); }

	std::string saveGamePath;
};

int main()
{
	Log(LogLevel::Info) << "CK3 to EU5 Converter - Version 0.1";
	Log(LogLevel::Info) << "Built with commonItems & Fronter technology.";

	if(commonItems::DoesFileExist("configuration.txt"))
	{
		Log(LogLevel::Info) << "Reading configuration...";
		ConverterConfig config;
		try { config.load("configuration.txt"); }
		catch(const std::exception& e)
		{
			Log(LogLevel::Error) << "Error loading configuration: " << e.what();
			return -1;
		}
	}
	else { Log(LogLevel::Warning) << "No configuration.txt found. This is expected if running directly without Frontend."; }

	Log(LogLevel::Progress) << "0%";
	Log(LogLevel::Info) << "Initializing World...";

	Log(LogLevel::Progress) << "50%";
	Log(LogLevel::Info) << "Converting Provinces...";

	Log(LogLevel::Progress) << "100%";
	Log(LogLevel::Info) << "Outputting Mod...";

	Log(LogLevel::Info) << "Conversion Complete.";

	return 0;
}