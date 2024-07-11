from datetime import datetime, timezone

naive_str = "2024-07-10T19:14:40"
aware_str = "2024-07-10T19:14:40+02:00"

naive_dt = datetime.fromisoformat(naive_str)
aware_dt = datetime.fromisoformat(aware_str)

print(naive_dt.tzinfo)
print(aware_dt.tzinfo)
