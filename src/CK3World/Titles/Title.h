#ifndef CK3_TITLE_H
#define CK3_TITLE_H
#include "Color.h"
#include "Date.h"
#include "Parser.h"
#include <set>

namespace CK3 {
	class Character;
	class CoatOfArms;
	class LandedTitles;

	enum class LEVEL {
		BARONY,
		COUNTY,
		DUCHY,
		KINGDOM,
		EMPIRE,
		HEGEMONY
	};

	static std::map<int, LEVEL> IntToLevel{
		{0, LEVEL::BARONY},
		{1, LEVEL::COUNTY},
		{2, LEVEL::DUCHY},
		{3, LEVEL::KINGDOM},
		{4, LEVEL::EMPIRE},
		{5, LEVEL::HEGEMONY}
	};
	static std::map<LEVEL, int> LevelToInt{
		{LEVEL::BARONY, 0},
		{LEVEL::COUNTY, 1},
		{LEVEL::DUCHY, 2},
		{LEVEL::KINGDOM, 3},
		{LEVEL::EMPIRE, 4},
		{LEVEL::HEGEMONY, 5}
	};

	class Title : commonItems::parser {
	public:
		Title(std::istream &theStream, long long theID);

		[[nodiscard]] auto        getID() const { return ID; }
		[[nodiscard]] auto        isTheocraticLease() const { return theocraticLease; }
		[[nodiscard]] auto        isCountyCapitalBarony() const { return cCapitalBarony; }
		[[nodiscard]] auto        isDuchyCapitalBarony() const { return dCapitalBarony; }
		[[nodiscard]] auto        isHREEmperor() const { return HREEmperor; }
		[[nodiscard]] auto        isInHRE() const { return inHRE; }
		[[nodiscard]] auto        isThePope() const { return thePope; }
		[[nodiscard]] auto        isElectorate() const { return electorate; }
		[[nodiscard]] auto        isHolderCapital() const { return holderCapital; }
		[[nodiscard]] auto        isHRECapital() const { return HRECapital; }
		[[nodiscard]] auto        isCustomTitle() const { return customTitle; }
		[[nodiscard]] auto        isRenamed() const { return renamed; }
		[[nodiscard]] const auto &getName() const { return name; }
		[[nodiscard]] const auto &getDisplayName() const { return displayName; }

		[[nodiscard]] std::optional<std::string> getAlteredName() const;

		[[nodiscard]] const auto &getAdjective() const { return adjective; }
		[[nodiscard]] const auto &getCreationDate() const { return creationDate; }
		[[nodiscard]] const auto &getHistoryGovernment() const { return historyGovernment; }
		[[nodiscard]] const auto &getCapital() const { return capital; }
		[[nodiscard]] const auto &getDFLiege() const { return dfLiege; }
		[[nodiscard]] const auto &getDJLiege() const { return djLiege; }
		[[nodiscard]] const auto &getDFVassals() const { return dfVassals; }
		[[nodiscard]] const auto &getDJVassals() const { return djVassals; }
		[[nodiscard]] const auto &getHeirs() const { return heirs; }
		[[nodiscard]] const auto &getClaimants() const { return claimants; }
		[[nodiscard]] const auto &getElectors() const { return electors; }
		[[nodiscard]] const auto &getLaws() const { return laws; }
		[[nodiscard]] const auto &getHolder() const { return holder; }
		[[nodiscard]] const auto &getCoA() const { return coa; }
		[[nodiscard]] const auto &getClay() const { return clay; }
		[[nodiscard]] const auto &getOwnedDFCounties() const { return ownedDFCounties; }
		[[nodiscard]] const auto &getOwnedDJCounties() const { return ownedDJCounties; }
		[[nodiscard]] const auto &getHoldingTitle() const { return holdingTitle; }
		[[nodiscard]] const auto &getGeneratedLiege() const { return generatedLiege; }
		[[nodiscard]] const auto &getGeneratedVassals() const { return generatedVassals; }
		[[nodiscard]] const auto &getPreviousHolders() const { return previousHolders; }

		[[nodiscard]] std::optional<commonItems::Color> getColor() const;

		[[nodiscard]] bool isLandless() const;

		[[nodiscard]] bool areDFLiegeAndDJLiegeSet() const;

		[[nodiscard]] bool areDFLiegeAndDJLiegeSame() const;

		[[nodiscard]] bool isHolderSet() const { return holder != std::nullopt; }
		[[nodiscard]] bool isHolderLinked() const { return holder && holder->second; }

		[[nodiscard]] bool doesHolderHaveCharacterDomain() const;

		[[nodiscard]] std::set<std::string> getTitleNamesFromHolderDomain() const;

		[[nodiscard]] LEVEL getLevel() const;

		[[nodiscard]] double getDevelopment() const;

		// linkage
		void loadCoat(const std::pair<long long, std::shared_ptr<CoatOfArms> > &coat) { coa = coat; }
		void loadDFLiege(const std::pair<long long, std::shared_ptr<Title> > &DFLiege) { dfLiege = DFLiege; }
		void loadDJLiege(const std::pair<long long, std::shared_ptr<Title> > &DJLiege) { djLiege = DJLiege; }
		void loadDFVassals(const std::map<long long, std::shared_ptr<Title> > &DFVassals) { dfVassals = DFVassals; }

		void addDFVassals(const std::map<long long, std::shared_ptr<Title> > &DFVassals) {
			dfVassals.insert(DFVassals.begin(), DFVassals.end());
		}

		void loadDJVassals(const std::map<long long, std::shared_ptr<Title> > &DJVassals) { djVassals = DJVassals; }
		void loadHolder(const std::pair<long long, std::shared_ptr<Character> > &theHolder) { holder = theHolder; }

		void loadHeirs(const std::vector<std::pair<long long, std::shared_ptr<Character> > > &theHeirs) {
			heirs = theHeirs;
		}

		void loadPreviousHolders(const std::vector<std::pair<long long, std::shared_ptr<Character> > > &theHolders) {
			previousHolders = theHolders;
		}

		void loadClaimants(const std::map<long long, std::shared_ptr<Character> > &theClaimants) {
			claimants = theClaimants;
		}

		void loadElectors(const std::map<long long, std::shared_ptr<Character> > &theElectors) {
			electors = theElectors;
		}

		void loadClay(const std::shared_ptr<LandedTitles> &theClay) { clay = theClay; }

		void loadOwnedDFCounties(const std::map<std::string, std::shared_ptr<Title> > &theOwnedCounties) {
			ownedDFCounties = theOwnedCounties;
		}

		void loadOwnedDJCounties(const std::map<std::string, std::shared_ptr<Title> > &theOwnedCounties) {
			ownedDJCounties = theOwnedCounties;
		}

		// processing
		void setDynamicLevel(LEVEL theLevel) { dynamicLevel = theLevel; }

		void cleanUpDisplayName();

		[[nodiscard]] int flagDeJureHREProvinces();

		void brickTitle();

		void grantIndependence();

		void resetDFLiege() { dfLiege.reset(); }
		void resetDJLiege() { djLiege.reset(); }
		void setHREEmperor() { HREEmperor = true; }
		void setInHRE() { inHRE = true; }

		void dropTitleFromDFVassals(long long titleID);

		void setThePope() { thePope = true; }
		void setCustomTitle() { customTitle = true; }

		void congregateDFCounties();

		void congregateDJCounties();

		void loadGeneratedLiege(const std::pair<std::string, std::shared_ptr<Title> > &liege) {
			generatedLiege = liege;
		}

		void addGeneratedVassal(const std::pair<std::string, std::shared_ptr<Title> > &theVassal) {
			generatedVassals.insert(theVassal);
		}

		void loadHoldingTitle(const std::pair<std::string, std::shared_ptr<Title> > &theTitle) {
			holdingTitle = theTitle;
		}

		void setElectorate() { electorate = true; }

		void relinkDeFactoVassals();

		void setHolderCapital() { holderCapital = true; }
		void setHRECapital() { HRECapital = true; }
		void clearGeneratedVassals() { generatedVassals.clear(); }

		[[nodiscard]] std::map<std::string, std::shared_ptr<Title> > coalesceDFCounties() const;

		[[nodiscard]] std::map<std::string, std::shared_ptr<Title> > coalesceDJCounties() const;

	private:
		void registerKeys();

		parser nameParser;

		long long                                                          ID = 0;
		std::pair<long long, std::shared_ptr<Title> >                      capital;
		std::string                                                        name;
		std::string                                                        alteredName;
		std::string                                                        displayName;
		std::string                                                        adjective;
		std::string                                                        article;
		std::string                                                        historyGovernment;
		date                                                               creationDate;
		std::optional<std::pair<long long, std::shared_ptr<CoatOfArms> > > coa;
		std::optional<std::pair<long long, std::shared_ptr<Title> > >      dfLiege;
		std::optional<std::pair<long long, std::shared_ptr<Title> > >      djLiege;
		std::optional<std::pair<long long, std::shared_ptr<Character> > >  holder;
		std::map<long long, std::shared_ptr<Title> >                       dfVassals;
		std::map<long long, std::shared_ptr<Title> >                       djVassals;
		std::vector<std::pair<long long, std::shared_ptr<Character> > >    heirs;
		std::map<long long, std::shared_ptr<Character> >                   claimants;
		std::map<long long, std::shared_ptr<Character> >                   electors;
		bool                                                               theocraticLease = false;
		std::set<std::string>                                              laws;
		bool                                                               cCapitalBarony = false;
		bool                                                               dCapitalBarony = false;
		std::shared_ptr<LandedTitles>                                      clay;
		bool                                                               HREEmperor  = false;
		bool                                                               inHRE       = false;
		bool                                                               thePope     = false;
		bool                                                               customTitle = false;
		bool                                                               renamed     = false;
		std::map<std::string, std::shared_ptr<Title> >                     ownedDFCounties;
		std::map<std::string, std::shared_ptr<Title> >                     ownedDJCounties;
		std::optional<std::pair<std::string, std::shared_ptr<Title> > >    generatedLiege;
		std::map<std::string, std::shared_ptr<Title> >                     generatedVassals;
		std::pair<std::string, std::shared_ptr<Title> >                    holdingTitle;
		bool                                                               electorate = false;
		bool                                                               landless   = false;
		std::optional<commonItems::Color>                                  color;
		std::vector<std::pair<long long, std::shared_ptr<Character> > >    previousHolders;
		bool                                                               holderCapital = false;
		bool                                                               HRECapital    = false;
		std::optional<LEVEL>                                               dynamicLevel;
	};
} // namespace CK3

#endif // CK3_TITLE_H
