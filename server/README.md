# [FastAPI](https://fastapi.tiangolo.com/) Server

Der REST-Server für unseren Markt. Braucht zum funktionieren die Logik als `.so` Library im `server/` directory. Der Server dient nur als Schnittstelle zwischen Client und Logik, er kümmert sich nur um die Datenvalidierung und Fehlerbehandlung. Dafür nutzt der Server [pydantic models](https://docs.pydantic.dev/latest/) und [fastapi exception handler](https://fastapi.tiangolo.com/tutorial/handling-errors/#install-custom-exception-handlers),

## Authenfizierung

Der Server unterstützt die Authentifizierung der Nutzer über das Security Modul von, um genau zu sein die die [Implementierung von OAuth2 Password Bearers](https://fastapi.tiangolo.com/tutorial/security/simple-oauth2/). Der Server erwartet das Nutzer sich mit Nutzername + Password registrieren (`/register`) oder einloggen (`/login`). Dabei kriegen sie als Response einen Bearer-Token, den sie bei allen Requests, die eine Authentifizierung erfordern, im Authorization-Header mitschicken müssen. Mit diesem kann die C++ Logik dem Sender einem User zuordnen.

### Sicherheitsbedenken

Da Nutzername, Passwort und Token im Klartext über HTTP versendet werden, bietet diese Authentifizierung so gut wie über keine Sicherheit.

## Anforderung

- Alle Schnittstellen in der [API Reference implementieren](#api-reference) implementieren
- Argumente des Senders vorbereiten, auf eventuelle Validierungsfehler wird hier schon reagiert und entsprechend ein Validierungsfehler (422) gesendet
- Entsprechenden C++ Code über die pybindings aufrufen
- Eventuelle Fehlermeldungen, wie zum Beispiel Authorisierungsfehler, du bist Pleite oder du hast nicht genug Warengüter zum verkaufen, zurück an den Sender senden

## API Reference

Server starten und dann unter [127.0.0.1:8000/docs](127.0.0.1:8000/docs) oder unter [docs](/docs/static_api_reference.html). Die Documentation wird automatisch von FastAPI generiert.

## Installation
1. Follow Installation instructions for the required pybindings
2. Install dependencies for the server with your favorite python version (tested for 3.12.4)
```bash
pip install -e server
```
3. Run server (with optional `--reload`)
```bash
uvicorn trading_server.__main__:app --port 8080 --reload
# if this doesnt work
uvicorn trading_server.__main__:app --port 8080 --reload
# or
python -m trading_server
```

## Build API Reference
```bash
# install redocly
npm install -g redocly
# get newest openapi specification (server needs to run)
wget localhost:8080/openapi.json -O server/openapi.json
# build docs
redocly build-docs server/openapi.json --output docs/static_api_reference.html
```