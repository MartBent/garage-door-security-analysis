# Introduction

This research explores how a garage door communicates with its remote control and how secure that connection is. Using an commercial 433 MHz garage door system, the study examines the hardware, radio communication, security mechanism, and possible vulnerabilities. The main question is: How does the communication with a garage door work, and how secure is it?

## Hardware
The remote control has several buttons connected to a small PCB. There’s no visible microcontroller, which suggests limited processing power or encryption. The manual of the tested system in question confirms that all models in this series share the same communication protocol and pairing process.

<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/a505f482-842f-424c-ad79-7c14797c6f9b" />
<img width="459" height="402" alt="Screenshot 2025-11-02 at 18 10 42" src="https://github.com/user-attachments/assets/098ff7ad-6e2f-4b7c-acee-23129d885e70" />


# Communication Method

Garage doors use 433 MHz radio signals, a common frequency for low-power wireless devices like car remotes and toys. Using a HackRF SDR and tools like GNU Radio and Gqrx, tests confirmed that the transmitter operates around 433.8 MHz.

<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/436eec10-60eb-4ad7-8c19-dc5689f768a8" />
<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/24550d56-ad4e-4e4e-acb5-56e5ca9940b9" />

# Communication Security

## Rolling Code

The system uses a rolling code algorithm where each press sends a unique code derived from a shared seed. This prevents simple replay attacks.
	•	Authorization: Only devices with the same seed are accepted.
	•	Authentication: Anyone can press the button; no identity check exists.
	•	Auditing: The receiver may detect brute-force attempts by logging failed tries.

The system is effective for low-power devices but can desynchronize if used out of range or too often.

## Rolling Code Analysis

To verify the rolling code, the transmitted binary was captured with GNU Radio Companion. with the following setup a .wav file can be generated holding the signal, which can be deocded using Inspectrum:

<img width="558" height="288" alt="image" src="https://github.com/user-attachments/assets/1e2eab69-e075-4bdf-85a9-edb9328dc92c" />

In Inspectrum, you can clearly see that a binary signal is modulated:

<img width="818" height="628" alt="image" src="https://github.com/user-attachments/assets/16996a45-e844-4f40-8449-33424394df30" />

The signal starts with a 12 pulses, looking like some sort of start condition. After a delay of 4ms the rest of the code is transmitted. 
When the button is held down, a delay of 16ms is forced between the signals, so the receiver can tell if a packet has been send completely:

<img width="973" height="282" alt="image" src="https://github.com/user-attachments/assets/65bd9049-1913-42a2-98ad-b2cb15af1504" />

The signal can be coverted to a binary format by using Inspectum's threshhold function:

<img width="1121" height="257" alt="image" src="https://github.com/user-attachments/assets/ae322bad-ce07-48b8-a973-fb780acdb124" />

The conversion results in a consistent signal pattern with 12 start pulses and 4 ms delays. Binary encoding depends on pulse duration (1 = full pulse, 0 = half pulse). Comparing multiple messages revealed changing bytes:

0xDF DB *59 59* 54 E4 A1 DB  
0xDF DB *6C C6* 54 E4 A1 DB  
0xDF DB *49 49* 54 E4 A1 DB

Only two bytes differ, confirming an 8-bit rolling section duplicated for error detection.

# Vulnerabilities

1. The Rolling code is only 8 bits long, which means the door can be brute forced with only a maximum of 255 retries.
2. After doing more practical tests, it seemed that the system does not respond to the rolling code at all, any random value can be set in the rolling code field triggering the opening of the door.

