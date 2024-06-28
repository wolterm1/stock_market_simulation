class IncorrectCredentials(Exception):
    pass


class UserAlreadyExists(Exception):
    pass


class InvalidToken(Exception):
    pass


class ProductNotFound(Exception):
    pass


class TransactionFailed(Exception):
    pass
