#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>  // for std::fixed and std::setprecision

using namespace std;

// Function to generate a random amount for bells (1 to 20 * hand size)
int generateBellValue(int handSize) {
    return (rand() % 20 + 1) * handSize;
}

// Function to simulate Rush Mode, returns total winnings from rush and updates rushCount and spinsUsed
int rushMode(int handSize, int& rushCount, int& spinsUsed) {
    rushCount++;
    int spinsLeft = 3;
    int totalSlots = 20; // 5 x 4 grid
    vector<int> bellValues(totalSlots, 0); // 0 means empty slot

    // Initial 5 bells with random values, randomly placed
    int placedBells = 0;
    while (placedBells < 5) {
        int pos = rand() % totalSlots;
        if (bellValues[pos] == 0) {
            bellValues[pos] = generateBellValue(handSize);
            placedBells++;
        }
    }

    while (spinsLeft > 0 && placedBells < totalSlots) {
        bool bellHit = false;

        // For each empty slot, simulate chance to get a bell (33% chance)
        for (int i = 0; i < totalSlots; ++i) {
            if (bellValues[i] == 0) {
                if ((rand() % 100) < 33) {  // 33% chance for a bell
                    bellValues[i] = generateBellValue(handSize);
                    placedBells++;
                    bellHit = true;
                }
            }
        }

        if (bellHit)
            spinsLeft = 3;
        else
            --spinsLeft;

        spinsUsed++;
    }

    // Sum all bell values
    int totalWinnings = 0;
    for (int val : bellValues)
        totalWinnings += val;

    return totalWinnings;
}

int main() {
    srand((unsigned int)time(0));

    ofstream resultFile("gambling_simulation_data.txt");
    ofstream csvDataFile("gambling_simulation_data.csv");

    csvDataFile << "Batch,Winning Tries,Time-Limit Wins,Total Won,Total Lost,Total Profit,Win Rate (%),Avg Profit per Win,Avg Spins per Try,Avg Rushes per Try\n";

    const int TryOnTryes = 50;             // How many simulation batches
    const int BellsOdds = 200;             // Chance to get 5 bells (1 in BellsOdds)
    const int TryFor = 10000;                 // How many tries per batch
    const int handSize = 1;                // RON per spin
    const int maxSpins = 100000;           // Max spins per try
    const int profitTarget = 150;          // Stop if profit reaches RON
    const int lossLimit = -40;             // Stop if losses reach RON
    const int secondsPerSpin = 4;          // Each spin takes secondsPerSpin seconds 
    const int maxMinutes = 3;              // Max minutes to gamble
    const int maxAllowedTime = maxMinutes * 60;     // Time limit to gamble in seconds

    resultFile << "=== Simulation Parameters ===\n";
    resultFile << "Simulations: " << TryOnTryes << "\n";
    resultFile << "Five bells odds: 1 in " << BellsOdds << "\n";
    resultFile << "Batches of: " << TryFor << " tryes\n";
    resultFile << "HandSize: " << handSize << " RON\n";
    resultFile << "Profit target: " << profitTarget << "\n";
    resultFile << "Loss limit: " << lossLimit << "\n";
    resultFile << "Time limit: " << maxMinutes << " min \n";

    resultFile << "\nSimulation start.\n\n";

    for (int batch = 1; batch <= TryOnTryes; ++batch) {
        int currentTry = 1;
        int totalProfitAllTries = 0;
        int winningTries = 0;
        int timeLimitWinningTries = 0;
        int totalSpinsAllTries = 0;
        int totalRushesAllTries = 0;
        int totalLoss = 0;

        string filename = "simulation_results_" + to_string(batch) + ".csv";
        ofstream dataFile(filename);
        dataFile << "Spin,Total Invested,Total Won,Profit/Loss\n";

        for (currentTry = 1; currentTry <= TryFor; ++currentTry) {
            int totalInvested = 0;
            int totalWon = 0;
            int rushCount = 0;
            int spinsUsedInRush = 0;
            int simulatedTime = 0;

            int profit = 0;
            int spin = 0;

            for (spin = 1; spin <= maxSpins; ++spin) {
                totalInvested += handSize;
                simulatedTime += secondsPerSpin;
                
                // Chance to get 5 bells (1 in 200)
                if ((rand() % BellsOdds) == 0) {
                    int rushWinnings = rushMode(handSize, rushCount, spinsUsedInRush);
                    totalWon += rushWinnings;
                }

                profit = totalWon - totalInvested;

                dataFile << spin << "," << totalInvested << "," << totalWon << "," << profit << "\n";

                if (profit >= profitTarget || profit <= lossLimit || simulatedTime >= maxAllowedTime) {
                    dataFile << "\n\n";
                    break;
                }
            }

            totalSpinsAllTries += spin;
            totalRushesAllTries += rushCount;

            dataFile << "Simulated time used: " << simulatedTime / 60 << " minutes " << simulatedTime % 60 << " seconds\n";
            if (profit >= profitTarget) {
                dataFile << "THIS IS A PROFIT TARGET WINNER | Try #" << currentTry << " | PROFIT: +" << profit << "\n\n";
                totalProfitAllTries += profit;
                winningTries++;
            }
            else if (simulatedTime >= maxAllowedTime && profit > 0) {
                dataFile << "TIME LIMIT POSITIVE SESSION | Try #" << currentTry << " | PROFIT: +" << profit << "\n\n";
                totalProfitAllTries += profit;
                winningTries++;
                timeLimitWinningTries++;
            }

        }

        dataFile.close();

        totalLoss = (TryFor - winningTries) * lossLimit;

        resultFile << "=== Batch #" << batch << " Summary ===\n";
        resultFile << "Total tries: " << TryFor << "\n";
        resultFile << "Winning tries: " << winningTries << "\n";
        resultFile << "Winning time run out tries: " << timeLimitWinningTries << "\n";
        resultFile << "Won: +" << totalProfitAllTries * 1.0 << " RON\n";
        resultFile << "Lost: " << totalLoss * 1.0 << " RON\n";
        resultFile << "Total: " << totalProfitAllTries + totalLoss * 1.0 << " RON\n";
        resultFile << fixed << setprecision(2);
        resultFile << "Win rate: " << (winningTries * 100.0 / TryFor) << "%\n";
        if (winningTries > 0)
            resultFile << "Average profit per winning try: +" << (totalProfitAllTries * 1.0 / winningTries) << " RON\n";
        else
            resultFile << "Average profit per winning try: N/A\n";

        resultFile << "Average spins per try: " << (totalSpinsAllTries * 1.0 / TryFor) << "\n";
        resultFile << "Average rushes per try: " << (totalRushesAllTries * 1.0 / TryFor) << "\n\n";

        csvDataFile << batch << ',' <<
                    winningTries << ',' <<
                    timeLimitWinningTries << ',' <<
                    totalProfitAllTries * 1.0 << ',' <<
                    totalLoss * 1.0 << ',' <<
                    totalProfitAllTries + totalLoss * 1.0 << ',' <<
                    (winningTries * 100.0 / TryFor) << ',' <<
                    (totalProfitAllTries * 1.0 / winningTries) << ',' <<
                    (totalSpinsAllTries * 1.0 / TryFor) << ',' <<
                    (totalRushesAllTries * 1.0 / TryFor) << '\n'; 
    }

    resultFile.close();
    csvDataFile.close();

    cout << "Simulation complete. Results saved in CSV files.\n";
    return 0;
}
