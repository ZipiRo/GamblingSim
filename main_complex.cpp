/*
=============================================
    GamblingSimulator - Data
=============================================
    Author          : ZipiRo
    Date            : 6/30/2025
    Description     : 
        I wanted to simulate what would happen if you gamble on the game of slots
        and the program gives a lot of data of what happens when you chose to use 
        a certain hand size and some limits to stop gambling.

        Intended as a mathematical and statistical tool to analyze game 
        balance and player outcomes. Inspired by real-world casino behavior.
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;

const bool CSVFiles = false; // If you want data for the every batch, I don't think you want it for this
int Odds = 330; // 330 is the goldylocks 110 is just money and > 330 is just loosing

int generateBellValue(int handSize)
{
    int roll = rand() % 100 + 1;
    if (roll <= 70)
        return handSize;

    return handSize * (roll % 20) + 1;
}

// Function to simulate Rush Mode
int rushMode(int handSize, int firstHandBellsCount, int &spinsUsed, int &rushTotalBellCount, ofstream &data)
{
    int spinsLeft = 3;
    int totalSlots = 20;                   // 5 x 4 grid
    vector<int> bellValues(totalSlots, 0); // 0 means empty slot

    // Initial 5 bells with random values, randomly placed
    int placedBells = 0;
    for (int i = 0; i < firstHandBellsCount; i++)
    {
        if (bellValues[i] == 0)
        {
            bellValues[i] = generateBellValue(handSize);
            placedBells++;
        }
    }

    while (spinsLeft > 0 && placedBells < totalSlots)
    {
        bool bellHit = false;

        // For each empty slot, simulate chance to get a bell (33% chance)
        for (int i = firstHandBellsCount; i < totalSlots; ++i)
        {
            if ((rand() % (Odds - 100)) < 10)
            {
                bellValues[i] = generateBellValue(handSize);
                placedBells++;
                bellHit = true;
            }
        }

        if (bellHit)
            spinsLeft = 3;
        else
            --spinsLeft;

        spinsUsed++;
    }

    rushTotalBellCount = placedBells;

    if (CSVFiles)
        data << "=== Bell Values From Rush ===\n";

    // Sum all bell values
    int totalWinnings = 0;
    for (int value : bellValues)
    {
        if (CSVFiles)
            data << "[" << value << "] ";

        totalWinnings += value;
    }

    if (CSVFiles)
        data << "\n=== Adding Up To: " << totalWinnings << "RON ===\n";

    return totalWinnings;
}

// Function to simulate Base Mode
int baseGameBonus(int handSize, bool &rushTriggered, int &firstHandBellsCount)
{
    const int symbolCount = 20;
    vector<int> symbols(symbolCount); // 0 = nothing, 1 = Bell, 2 = Crown, 3 = Star, 4 = Heart, 5 = Diamond

    int bellCount = 0;
    int crownCount = 0;
    int starCountInRow = 0;
    int heartCount = 0;
    int diamondCount = 0;

    // Generate 5 random symbols
    for (int i = 0; i < symbolCount; ++i)
    {
        int roll = rand() % Odds;
        if (roll < 20)
        {
            symbols[i] = 1;
            bellCount++;
        }
        else if (roll < 35)
        {
            symbols[i] = 2;
            crownCount++;
        }
        else if (roll < 45)
        {
            symbols[i] = 3;
        }
        else if (roll < 53)
        {
            symbols[i] = 4;
            heartCount++;
        }
        else if (roll < 58)
        {
            symbols[i] = 5;
            diamondCount++;
        }
        else
        {
            symbols[i] = 0;
        }
    }

    int payout = 0;

    // Rush Mode trigger
    if (bellCount >= 5)
    {
        firstHandBellsCount = bellCount;
        rushTriggered = true;
        return 0;
    }

    // Crown bonus
    if (crownCount >= 4)
        payout += 20 * handSize;

    // Star streak bonus
    int currentStarStreak = 0;
    for (int s : symbols)
    {
        if (s == 3)
            currentStarStreak++;
        else
            currentStarStreak = 0;

        if (currentStarStreak >= 3)
        {
            payout += 15 * handSize;
            break;
        }
    }

    // Heart bonus
    if (heartCount == symbolCount)
        payout += 30 * handSize;

    // Diamond bonus
    if (diamondCount >= 2)
        payout += 20 * handSize;

    return payout;
}

int main()
{
    srand((unsigned int)time(0));

    ofstream csvBigResult("Result/gambling_simulation_important_data.csv");
    csvBigResult << "Odd,Average Win,Average Lost,Average Profit\n";

    for (int i = 300; i <= 301; i+=10)
    {
        Odds = i;

        ofstream resultFile("Result/gambling_simulation_data.txt");
        ofstream csvDataFile("Result/gambling_simulation_data.csv");

        csvDataFile << "Batch,Winning Tries,Time-Limit Wins,Total Won,Total Lost,Total Profit,Win Rate (%),Avg Profit per Win,Avg Spins per Try,Avg Rushes per Try\n";

        const int TryOnTryes = 10;                // How many simulation batches
        const int TryFor = 100;                      // How many tries per batch
        const int handSize = 100;                     // RON per spin
        const int maxSpins = 100000;                // Max spins per try
        const int profitTarget = 200;               // Stop if profit reaches RON
        const int lossLimit = -3000;                  // Stop if losses reach RON
        const int secondsPerSpin = 5;               // Each spin takes secondsPerSpin seconds
        const int maxMinutes = 3;                   // Max minutes to gamble
        const int maxAllowedTime = maxMinutes * 60; // Time limit to gamble in seconds

        int averageWon = 0;
        int averageLost = 0;
        int averageProfit = 0;

        resultFile << "=== Simulation Parameters ===\n";
        resultFile << "Simulations: " << TryOnTryes << "\n";
        resultFile << "Odds: " << Odds << "\n";
        resultFile << "Batches of: " << TryFor << " tryes\n";
        resultFile << "HandSize: " << handSize << " RON\n";
        resultFile << "Profit target: " << profitTarget << "\n";
        resultFile << "Loss limit: " << lossLimit << "\n";
        resultFile << "Time limit: " << maxMinutes << " min \n";

        resultFile << "\nSimulation start.\n\n";

        for (int batch = 1; batch <= TryOnTryes; ++batch)
        {
            int currentTry = 1;
            int totalProfitAllTries = 0;
            int winningTries = 0;
            int timeLimitWinningTries = 0;
            int totalSpinsAllTries = 0;
            int totalRushesAllTries = 0;
            int totalLossAllTries = 0;

            string filename = "Result/simulation_results_" + to_string(batch) + ".csv";
            ofstream dataFile;

            if (CSVFiles)
                dataFile.open(filename);

            for (currentTry = 1; currentTry <= TryFor; ++currentTry)
            {
                if (CSVFiles)
                {
                    dataFile << "Spin,Total Invested,Total Won,Profit/Loss\n";
                }

                int totalInvested = 0;
                int totalWon = 0;
                int rushCount = 0;
                int spinsUsedInRush = 0;
                int simulatedTime = 0;

                int profit = 0;
                int maxProfit = 0;
                int spin = 0;
                int rushTotalBellsCount = 0;

                for (spin = 1; spin <= maxSpins; ++spin)
                {
                    bool rushTriggered = false;
                    int firstHandBellsCount = 0;
                    totalInvested += handSize;
                    simulatedTime += secondsPerSpin;

                    int basePayout = baseGameBonus(handSize, rushTriggered, firstHandBellsCount);
                    totalWon += basePayout;

                    if (rushTriggered)
                    {
                        int rushWinnings = rushMode(handSize, firstHandBellsCount, spinsUsedInRush, rushTotalBellsCount, dataFile);
                        totalWon += rushWinnings;
                        rushCount++;
                    }

                    profit = totalWon - totalInvested;
                    if (maxProfit < profit)
                        maxProfit = profit;

                    if (CSVFiles)
                        dataFile << spin << ",     " << totalInvested << ",       " << totalWon << ",       " << profit << "\n";

                    if (profit >= profitTarget || profit <= lossLimit || simulatedTime >= maxAllowedTime)
                    {
                        if (CSVFiles)
                            dataFile << "\n";
                        break;
                    }
                }

                totalSpinsAllTries += spin;
                totalRushesAllTries += rushCount;

                if (CSVFiles)
                    dataFile << "Simulated time used: " << simulatedTime / 60 << " minutes " << simulatedTime % 60 << " seconds\n";
                if (profit >= profitTarget)
                {
                    if (CSVFiles)
                        dataFile << "THIS IS A PROFIT TARGET WINNER | Try #" << currentTry << " | PROFIT: " << profit << " RON\n";
                    totalProfitAllTries += profit;
                    winningTries++;
                }
                else if (simulatedTime >= maxAllowedTime && profit >= 0)
                {
                    if (CSVFiles)
                        dataFile << "TIME LIMIT POSITIVE SESSION | Try #" << currentTry << " | PROFIT: " << profit << " RON\n";
                    totalProfitAllTries += profit;
                    winningTries++;
                    timeLimitWinningTries++;
                }
                if (CSVFiles)
                    dataFile << "MAX PROFIT: " << maxProfit << " RON | RUSH SPINS: " << rushCount << " | RUSH TOTAL BELLS COUNT: " << rushTotalBellsCount << "\n\n";
            }

            if (CSVFiles)
                dataFile.close();

            totalLossAllTries = (TryFor - winningTries) * lossLimit;

            resultFile << fixed << setprecision(2);
            resultFile << "=== Batch #" << batch << " Summary ===\n";
            resultFile << "Total tries: " << TryFor << "\n";
            resultFile << "Winning tries: " << winningTries << "\n";
            resultFile << "Winning time run out tries: " << timeLimitWinningTries << "\n";
            resultFile << "Won: +" << totalProfitAllTries * 1.0 << " RON\n";
            resultFile << "Lost: " << totalLossAllTries * 1.0 << " RON\n";
            resultFile << "Total: " << totalProfitAllTries + totalLossAllTries * 1.0 << " RON\n";
            resultFile << "Win rate: " << (winningTries * 100.0 / TryFor) << "%\n";
            if (winningTries > 0)
                resultFile << "Average profit per winning try: +" << (totalProfitAllTries * 1.0 / winningTries) << " RON\n";
            else
                resultFile << "Average profit per winning try: N/A\n";

            resultFile << "Total spins: " << totalSpinsAllTries << "\n";
            resultFile << "Total rushes: " << totalRushesAllTries << "\n";
            resultFile << "Average spins per try: " << (totalSpinsAllTries * 1.0 / TryFor) << "\n";
            resultFile << "Average rushes per try: " << (totalRushesAllTries * 1.0 / TryFor) << "\n\n";

            csvDataFile << batch << ',' << winningTries << ',' << timeLimitWinningTries << ',' << totalProfitAllTries * 1.0 << ',' << totalLossAllTries * 1.0 << ',' << totalProfitAllTries + totalLossAllTries * 1.0 << ',' << (winningTries * 100.0 / TryFor) << ',' << (totalProfitAllTries * 1.0 / winningTries) << ',' << (totalSpinsAllTries * 1.0 / TryFor) << ',' << (totalRushesAllTries * 1.0 / TryFor) << '\n';

            averageWon += totalProfitAllTries;
            averageLost += totalLossAllTries;
            averageProfit += totalProfitAllTries + totalLossAllTries;
        }

        averageLost /= TryOnTryes;
        averageWon /= TryOnTryes;
        averageProfit /= TryOnTryes;

        resultFile << "\n=== Simulation Averages === \n";
        resultFile << "Average Won: " << averageWon << " RON | Average Lost: " << averageLost << " RON | Average Profit: " << averageProfit << " RON \n\n";

        csvBigResult << Odds << ',' << averageWon << ',' << averageLost << ',' << averageProfit << '\n';
        resultFile.close();
        csvDataFile.close();
    }

    csvBigResult.close();
    cout << "Simulation complete. Results saved in CSV files.\n";
    return 0;
}
