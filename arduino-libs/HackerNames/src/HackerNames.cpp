#include "HackerNames.h"

const char *verbs[] {
    "anonymous",
    "bruteforce",
    "phishing",
    "cracking",
    "exploiting",
    "dos",
    "ddos",
    "doxing",
    "hacking",
    "remoteaccess",
    "spoofing",
    "leet",
    "patched",
    "reversing",
    "pentesting",
    "outofbounds",
    "overflowing",
    "jit",
    "compiling",
    "unpatched",
    "open",
    "bypassing",
    "secure",
    "advanced",
    "lowtech",
};

const char *names[] {
    "adware",
    "antisec",
    "backdoor",
    "blackhat",
    "whitehat",
    "bot",
    "botnet",
    "code",
    "compiler",
    "coolie",
    "exploit",
    "privesc",
    "sandboxescape",
    "vulnerability",
    "bug",
    "dos",
    "ddos",
    "firewall",
    "hacktivist",
    "hash",
    "ip",
    "irc",
    "logicbomb",
    "anonymous",
    "lulzsec",
    "malware",
    "nsa",
    "gchq",
    "cse",
    "gcsb",
    "asd",
    "payload",
    "packet",
    "scriptkiddy",
    "rootkit",
    "spam",
    "spware",
    "apt",
    "crimeware",
    "trojanhorse",
    "virus",
    "worm",
    "zeroday",
    "android",
    "unix",
    "linux",
    "osx",
    "macos",
    "windows",
    "xp",
    "vista",
    "oracle",
    "microsoft",
    "apple",
    "google",
    "amazon",
    "penten",
    "infosect",
    "kylie",
    "silvo",
    "javascript",
    "java",
    "swift",
    "rust",
    "anitpattern",
    "bufferoverflow",
    "ipad",
    "iphone",
    "ipod",
    "python",
    "smb",
    "rdp",
    "bonjour",
    "siri",
    "cortana",
    "alexia",
    "edge",
    "ie",
    "chrome",
    "chromium",
    "netscape",
    "facebook",
    "firefox",
    "mozilla",
    "whatsapp",
    "signal",
    "wire",
    "telegram",
    "msn",
    "wifi",
    "bluetooth",
    "nfc",
    "qrcode",
    "barcode",
    "swipecard",
    "proxcard",
    "ghidra",
    "ida",
    "apt",
    "sysadmin",
};

static size_t hacker_name_verbs_count(void)
{
    return sizeof(verbs)/sizeof(verbs[0]);
}

static size_t hacker_name_names_count(void)
{
    return sizeof(names)/sizeof(names[0]);
}

size_t hacker_name_count(void)
{
    return hacker_name_verbs_count() * hacker_name_names_count();
}

void hacker_name(size_t index, char *out, size_t out_size)
{
    index = index % hacker_name_count();
    size_t name_index = index / hacker_name_verbs_count();
    size_t verb_index = index % hacker_name_verbs_count();

    size_t char_index = 0;
    char letter = '\0';
    while ( out_size > 1 )
    {
        letter = verbs[verb_index][char_index];
        if ( letter == '\0' )
        {
            break;
        }
        *out = letter;

        char_index++;
        out++;
        out_size--;
    }

    if ( out_size > 1 )
    {
        *out = '_';
        out++;
        out_size--;
    }

    char_index = 0;
    letter = '\0';
    while ( out_size > 1 )
    {
        letter = names[name_index][char_index];
        if ( letter == '\0' )
        {
            break;
        }
        *out = letter;

        char_index++;
        out++;
        out_size--;
    }

    if ( out_size > 0 )
    {
        *out = '\0';
    }
}
