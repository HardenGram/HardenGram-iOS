#pragma once

#include <sstream>
#include <fstream>
#include <optional>

#include "Highlight.h"

struct Buffer {
    const std::string &content;
    int64_t offset = 0;
};

class LanguageTree : public std::enable_shared_from_this<LanguageTree>
{
public:
    LanguageTree() = default;

    void load(const std::string& content);

    // Rare/obscure languages can be disabled by default for hardening; the
    // Debug menu can re-enable them (with a warning) at runtime.
    static bool allowRareLanguages;
    static void setAllowRareLanguages(bool value) { allowRareLanguages = value; }
    static bool areRareLanguagesAllowed() { return allowRareLanguages; }

    // Whether a language is considered "rare" (excluded unless explicitly
    // re-enabled). Centralised here so find()/keys() agree.
    static bool isRareLanguage(const std::string& key);

    const Pattern* resolvePattern(size_t path);
    const Grammar* resolveGrammar(size_t path);

    std::map<std::string, std::string> keys() const
    {
        std::map<std::string, std::string> keys;

        for (const auto& kv : m_languages)
        {
            if (kv.second.first.empty())
            {
                continue;
            }

            if (!allowRareLanguages && isRareLanguage(kv.first))
            {
                continue;
            }

            keys.emplace(kv.first, kv.second.first);
        }

        return keys;
    }

    const Grammar* find(const std::string& key) const
    {
        if (!allowRareLanguages && isRareLanguage(key))
        {
            return nullptr;
        }

        const auto& value = m_languages.find(key);
        if (value != m_languages.end())
        {
            return m_grammars[value->second.second].get();
        }

        return nullptr;
    }

private:
    void parseLanguages(Buffer &buffer);
    void parseGrammars(Buffer &buffer);
    void parsePatterns(Buffer &buffer);

    std::map<std::string, std::pair<std::string, size_t>> m_languages;
    std::vector<std::shared_ptr<Grammar>> m_grammars;
    std::vector<std::shared_ptr<Pattern>> m_patterns;
};