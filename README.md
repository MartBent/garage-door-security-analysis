# Garage Door Security Research

> [!NOTE]  
> This research was conducted **for educational purposes only**.  
> No active systems were harmed or exploited during testing.  
> Sensitive or brand-specific details have been anonymized.

This repository contains a security analysis of a commercial **433 MHz garage door system**.  
It includes documentation of hardware inspection, radio signal capture, rolling code analysis, and proof-of-concept tests demonstrating potential vulnerabilities.

## Hardware

The remote control contains several buttons connected to a small PCB. There is no visible microcontroller, which suggests limited processing power or encryption capabilities.  
The manual for the tested system confirms that all models in this series share the same communication protocol and pairing process.

<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/a505f482-842f-424c-ad79-7c14797c6f9b" />  
<img width="459" height="402" alt="Screenshot 2025-11-02 at 18 10 42" src="https://github.com/user-attachments/assets/098ff7ad-6e2f-4b7c-acee-23129d885e70" />

# Communication Method

Garage doors use **433 MHz radio signals**, a common frequency for low-power wireless devices such as car remotes and toys. Using a **HackRF SDR** with tools like **GNU Radio** and **Gqrx**, tests confirmed that the transmitter operates around **433.8 MHz**.

<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/436eec10-60eb-4ad7-8c19-dc5689f768a8" />  
<img width="459" height="402" alt="image" src="https://github.com/user-attachments/assets/24550d56-ad4e-4e4e-acb5-56e5ca9940b9" />

# Communication Security

## Rolling Code

The system uses a **rolling code algorithm**, where each button press sends a unique code derived from a shared seed. This method helps prevent simple replay attacks.

- **Authorization:** Only devices with the same seed are accepted.  
- **Authentication:** Anyone can press the button; there is no identity verification.  
- **Auditing:** The receiver may detect brute-force attempts by logging failed tries.

This approach is efficient for low-power devices but can desynchronize if used too frequently or out of range.

## Rolling Code Analysis

To verify the rolling code, the transmitted binary data was captured with **GNU Radio Companion**. Using this setup, a `.wav` file was generated containing the signal, which was then decoded with **Inspectrum**:

<img width="558" height="288" alt="image" src="https://github.com/user-attachments/assets/1e2eab69-e075-4bdf-85a9-edb9328dc92c" />

In Inspectrum, the binary signal modulation is clearly visible:

<img width="818" height="628" alt="image" src="https://github.com/user-attachments/assets/16996a45-e844-4f40-8449-33424394df30" />

The signal begins with 12 pulses, serving as a possible start condition. After a 4 ms delay, the remaining data is transmitted.  
When the button is held down, a 16 ms delay is introduced between signals so the receiver can determine when a packet has been completely sent.

<img width="973" height="282" alt="image" src="https://github.com/user-attachments/assets/65bd9049-1913-42a2-98ad-b2cb15af1504" />

The signal can be converted to binary format using Inspectrum’s threshold function:

<img width="1121" height="257" alt="image" src="https://github.com/user-attachments/assets/ae322bad-ce07-48b8-a973-fb780acdb124" />

The conversion results in a consistent signal pattern with 12 start pulses and 4 ms delays.  
Binary encoding depends on pulse duration (1 = full pulse, 0 = half pulse). Comparing multiple transmissions reveals varying bytes:

0xDF DB **59 59** 54 E4 A1 DB \
0xDF DB **6C C6** 54 E4 A1 DB \
0xDF DB **49 49** 54 E4 A1 DB

Only two bytes differ, confirming an **8-bit rolling section** duplicated for error detection.

# Vulnerabilities

1. The rolling code is only 8 bits long, meaning the door could theoretically be brute-forced with a maximum of 255 attempts.  
2. Further practical testing showed that the system does not appear to validate the rolling code at all—any random value in the rolling code field triggers the door to open.  
3. The **source code used for these practical tests** can be found in the [`software/`](./software/) folder of this repository.

# Tools Used

- **HackRF One** – Software-defined radio for signal capture  
- **GNU Radio Companion** – Signal processing and data pipeline design  
- **Inspectrum** – Waveform analysis and binary decoding  
- **Gqrx SDR** – Spectrum visualization and recording  
