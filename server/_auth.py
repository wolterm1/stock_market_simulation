"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

from dataclasses import dataclass
import secrets


class UserNotFound(Exception):
    pass


class IncorrectPassword(Exception):
    pass


class InvalidToken(Exception):
    pass


users: list[tuple[int, str, str, str]] = [
    (1, "admin", "admin", None),
]


def authenticate_user(username: str, password: str) -> str:
    for i, (user_id, user_name, user_password, _) in enumerate(users):
        if user_name == username and user_password == password:
            token = secrets.token_hex(16)
            users[i] = (user_id, user_name, user_password, token)
            return token

    raise IncorrectPassword("Incorrect username or password")


def register(username: str, password: str):
    users.append((len(users) + 1, username, password, None))
    token = authenticate_user(username, password)
    return token


def logout(token: str):
    for i, (user_id, user_name, user_password, user_token) in enumerate(users):
        if user_token == token:
            users[i] = (user_id, user_name, user_password, None)
            return True

    raise InvalidToken("Token not found")


def find_user_by_token(token: str) -> int:
    for user_id, *_, user_token in users:
        if user_token != None and user_token == token:
            return user_id

    raise InvalidToken("User not found")
