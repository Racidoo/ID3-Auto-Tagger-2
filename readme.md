# ID3-Auto-Tagger-2


```bash
git clone https://github.com/Racidoo/ID3-Auto-Tagger-2.git
```

## Install Dependencies

The following packages are needed:
- curl
- nlohmann-json
- taglib
- wxWidgets
- yt-dlp
- ffmpeg

Installation on Ubuntu:
```bash
sudo apt install libcurl4-openssl-dev nlohmann-json3-dev libtag1-dev libwxgtk3.2-dev yt-dlp ffmpeg
```


## Create and insert OAuth Services

If not already done, the tool will provide an empty credentials file `backend/api/credentials.json`. This needs to be filled with personal credentials of the respective API-Service.

```json
{
    "OAuth": {
        "Google": {
            "auth_token": "YOUR_GOOGLE_API_KEY"
            
        },
        "Spotify": {
            "client_id": "YOUR_CLIENT_ID",
            "client_secret": "YOUR_CLIENT_SECRET",
            "token_uri": "https://accounts.spotify.com/api/token"
        }
    }
}
```

Alternatively, if no credentials are provided the tool will start in a restricted mode. In the settings menu the credentials can also be provided.

> The Auth Token for the Google API will not be checked instantly but rather with the first download. If the Token is not valid, the much slower fallback via yt-dlp will be used.