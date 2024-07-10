import auth
from auth import Account, authenticate_user


print(dir(auth))

displayname:str = "matthias"
MyAccount = Account("Matthias_wolter", "meinpassword")

