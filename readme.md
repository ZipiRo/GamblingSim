
# Gambling Simulation - Rush Mode Profit Analysis 🎰

This C++ project simulates a simplified gambling system inspired by modern slot machines featuring "Rush Mode" mechanics. It allows you to test and analyze profitability under different odds, hand sizes, and risk configurations.

---

## 📦 Features

✅ Base game with symbols like Bells, Crowns, Stars, Hearts, and Diamonds  
✅ Rush Mode with bell accumulation mechanic and respin system  
✅ Configurable odds, profit targets, loss limits, and hand size (bet amount)  
✅ Time constraints per gambling session  
✅ Detailed batch simulation results with optional CSV export  
✅ Statistical summary at the end of simulations  

---

## 🧮 How It Works

- You configure the odds, bet size, profit target, loss limit, and other parameters.
- The program simulates multiple gambling sessions (batches).
- Within each session, spins are performed until:
  - Profit target is reached
  - Loss limit is reached
  - Time limit expires
- If enough bells appear, **Rush Mode** is triggered, where additional bells can be collected for bonus profit.
- At the end of each batch, statistics like win rate, average profit, spins, and Rush Mode occurrences are displayed.

---

## ⚙️ Usage

### 1. Compile the Program

Example with `g++`:

```bash
g++ gambling_simulation.cpp -o simulation
```

### 2. Run the Simulation

```bash
./simulation
```

### 3. View Results

- Summary is saved in `gambling_simulation_data.txt`  
- Overall batch statistics in `gambling_simulation_data.csv`  
- (Optional) Per-batch detailed spin breakdown in `simulation_results_X.csv` if enabled  

---

## 📁 File Structure

| File                           | Description                                   |
|--------------------------------|-----------------------------------------------|
| `gambling_simulation.cpp`      | Main simulation source code                   |
| `gambling_simulation_data.txt` | Summary of all simulation batches             |
| `gambling_simulation_data.csv` | Overall batch statistics in CSV format        |
| `simulation_results_X.csv`     | (Optional) Per-batch detailed spin breakdowns |

---

## 🛠️ Configuration

Adjust these parameters directly in the code before compiling:

```cpp
const int Odds = 350;           // Lower = harder odds (affects symbol rarity)
const int handSize = 2;         // Bet amount per spin (€)
const int profitTarget = 1000;  // Profit target to stop session (€)
const int lossLimit = -30;      // Loss limit to stop session (€)
const int maxMinutes = 3;       // Max time per try in minutes
const int TryOnTryes = 10;      // Number of simulation batches
const int TryFor = 20;          // Tries per batch
const bool CSVFiles = true;     // Enable or disable detailed CSV output
```

---

## 🎯 Disclaimer

This project is intended for **educational and statistical purposes only**. It does **not** promote or encourage gambling in real-world settings. Gambling involves real financial risk — always play responsibly.

---

## 👨‍💻 Author

**Zipi Ro**  
C++ Developer | Programming Enthusiast  

---

## 🗒️ Notes

- The simulation assumes simplified probabilities and mechanics for statistical modeling.
- Real-world gambling machines may use far more complex and proprietary algorithms.
- The "Rush Mode" mechanic mimics bonus-style events seen in casino-style games.

