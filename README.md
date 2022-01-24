# Serial over LoRa
A full serial line connection between 2 devices using LoRa. Currently the code is set up for 1200bps with a 200bytes buffer, in theory we can get up to 2000bps safely without extra buffering code that i would need to figure out first.

# Things I wanna (try to) do
- Address Filtering
- Encryption
- Error Correction
- Resending (most) Lost Packets

# Things I need to fix
- The damn thing transmits 100% of the time and basically just jams the 433mhz frequency.
- Some reliability problem like random crashes etc.