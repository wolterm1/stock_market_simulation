"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""


class UserNotFound(Exception):
    pass


class IncorrectPassword(Exception):
    pass


def authenticate_user(username: str, password: str) -> str:
    return "Bearer 5gxl2.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ"


def register(username: str, password: str):
    return True


def logout():
    return True


def find_user_by_token(token: str) -> int:
    return 1
