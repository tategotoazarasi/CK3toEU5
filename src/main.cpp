#include "CK3World/World.h"
#include "Configuration/Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

int main(const int argc, const char *argv[]) {
	try {
		std::ofstream clearLog("log.txt");
		clearLog.close();

		commonItems::ConverterVersion converterVersion;
		converterVersion.loadVersion("configurables/version.txt");
		Log(LogLevel::Info) << "CK3toEU5 - " << converterVersion;

		const auto configuration = std::make_shared<Configuration>(converterVersion);

		const CK3::World sourceWorld(configuration, converterVersion);

		Log(LogLevel::Info) << "* CK3 Mod Conversion Complete *";
		Log(LogLevel::Progress) << "100%";

		return 0;
	} catch (const std::exception &e) {
		Log(LogLevel::Error) << e.what();
		return -1;
	}
}
