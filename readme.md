<div align="center">
  <h1><code>Discord Token Grabber</code></h1>
  <p>
    Discord Token Grabber is a Python script that helps you find and extract tokens used by various applications on different operating systems. Tokens are commonly used for authentication purposes in applications like Discord, web browsers, and more.
  </p>
</div>

---

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Supported Operating Systems](#supported-operating-systems)
- [Supported Softwares](#supported-softwares)

## Features

- No local caching
- Transfers via Discord webhook
- Searches for authorization tokens in multiple directories (Discord, Discord PTB, Discord Canary, Google chrome, Opera, Brave and Yandex)
- No external Python modules required
- Cross-platform support

## Getting Started

### Prerequisites

- Python 3.x

### Installation

1. Clone the repository:

```sh
git clone https://github.com/Fastiraz/Discord-Tokens-Grabber.git
```

2. Navigate to the project directory:

```sh
cd Discord-Tokens-Grabber
```

### Usage

1. Open a terminal or command prompt.
2. Navigate to the project directory where the script is located.
3. Run the script:

For **Windows**
```powershell
py grab.py
```

For **Unix/Linux**
```bash
python3 grab.py
```

4. The script will detect your operating system and attempt to find tokens for supported applications.

5. Tokens, if found, will be displayed in the terminal.

## Supported Operating Systems

- Linux
- macOS (Darwin)
- Windows

## Supported Softwares

- Google Chrome
- Discord
- Discord Canary
- Discord PTB
- Opera
- Brave
- Yandex

---

Please note that the paths to application data may vary depending on your specific setup, and permissions may be required to access certain directories.
