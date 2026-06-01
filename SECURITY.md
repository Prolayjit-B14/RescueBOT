# 🔒 Security Policy — RescueBOT

**Project:** RescueBOT — Autonomous Disaster Response & Rescue Robot
**Maintainer:** [@Prolayjit-B14](https://github.com/Prolayjit-B14) | Team BOT THINGS

---

## Supported Versions

The table below lists which versions of RescueBOT firmware and software currently receive security updates:

| Version / Branch | Status | Security Updates |
|:---|:---:|:---:|
| `main` (latest) | ✅ Active | ✅ Yes |
| Tagged releases (`v1.x`) | ✅ Active | ✅ Yes |
| Archived / older branches | ⚠️ Deprecated | ❌ No |

> Always use the **latest commit on `main`** or the **latest tagged release** for the most secure and stable version of RescueBOT.

---

## Scope — What Counts as a Security Issue

RescueBOT is an embedded IoT platform operating in physical rescue environments. Security issues in this context include:

### 🔴 Critical — Report Immediately
- **RF Spoofing Vulnerabilities:** Exploits that allow an unauthorized device to inject fake control packets into the nRF24L01+ control link (e.g., replaying captured packets to drive the UGV without operator consent).
- **LoRa Packet Injection:** Ability to inject forged sensor telemetry packets that trigger false alert conditions on the dashboard, leading to incorrect rescue decisions.
- **Wi-Fi Credential Exposure:** Hard-coded Wi-Fi SSID/passwords in any firmware file committed to the public repository.
- **Remote Code Execution:** Any vulnerability allowing arbitrary code execution on the ESP32 or Arduino over a network interface.

### 🟡 Moderate — Report Within 7 Days
- **Denial of Service on Control Link:** RF jamming patterns or protocol exploits that predictably disable the nRF24L01+ control link, triggering unintended fail-safe halts.
- **Dashboard Authentication Bypass:** The web dashboard (if deployed with authentication) being accessible without valid credentials.
- **Unsafe Default Firmware Configurations:** Firmware that ships with unsafe default thresholds, open HTTP endpoints without any access control, or debug modes enabled by default.

### 🟢 Low Priority / Informational
- Missing HTTPS on the dashboard (currently HTTP-only by design for LAN-only use).
- Absence of LoRa packet encryption (by design — plaintext telemetry for now; encryption is in the roadmap).

---

## Reporting a Vulnerability

> ⚠️ **DO NOT open a public GitHub Issue for security vulnerabilities.** This could alert malicious actors before a fix is available.

### Preferred Reporting Channel

**Contact the maintainer privately:**
- **GitHub:** [@Prolayjit-B14](https://github.com/Prolayjit-B14) — use GitHub's private vulnerability reporting feature (Security tab → "Report a vulnerability").
- **Email:** Available on the GitHub profile page.

### What to Include in Your Report

Please provide as much of the following as possible:

```
1. Vulnerability type (e.g., RF injection, credential exposure)
2. Affected component (firmware file, dashboard, LoRa link, nRF link)
3. Firmware version or commit hash where the vulnerability exists
4. Step-by-step reproduction steps
5. Proof of concept code or captured packets (if applicable)
6. Your assessment of impact (what an attacker could achieve)
7. Suggested fix (optional, but appreciated)
```

---

## Response Timeline

| Stage | Target Time |
|:---|:---:|
| Initial acknowledgment of report | Within 48 hours |
| Severity assessment and response | Within 5 business days |
| Fix development (for critical issues) | Within 14 days |
| Public disclosure (coordinated) | After fix is released |

---

## Known Limitations & Accepted Risks

RescueBOT is a research and hackathon prototype. The following are **known design limitations** that are **not currently treated as vulnerabilities** but are documented for transparency:

| Limitation | Details | Mitigation Plan |
|:---|:---|:---|
| **No LoRa encryption** | Telemetry packets are plaintext ASCII. An observer with a 433 MHz receiver can decode packets. | AES-128 encryption via LoRa library planned in Phase 5. |
| **No nRF authentication** | The nRF24L01+ uses a shared 5-byte address pipe. Any device with the same address can receive control packets. | Proprietary rolling-code or HMAC payload signing planned. |
| **No dashboard authentication** | The web dashboard is open on the LAN. Intended for closed operational networks only. | Basic HTTP auth or token-based access planned for field deployment. |
| **No OTA firmware security** | No OTA update mechanism exists currently. Firmware is flashed via USB only. | Signed OTA with ESP32's secure boot planned for Phase 5. |

---

## Responsible Disclosure Policy

We follow a **coordinated disclosure model**:
1. Reporter submits vulnerability privately.
2. We develop and release a fix.
3. We publish a security advisory in the GitHub Security Advisory tab.
4. Reporter receives credit in the advisory (unless they prefer anonymity).

We ask that reporters allow us a minimum of **30 days** before public disclosure to allow time for fix development and deployment.

---

## Hall of Fame — Security Researchers

We recognize researchers who responsibly disclose vulnerabilities:

*No disclosures yet — be the first!*

---

*Last Updated: June 2026 | Policy maintained by Team BOT THINGS*
