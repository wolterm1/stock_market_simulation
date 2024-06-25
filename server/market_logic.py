"""THIS IS JUST A DEBUG FILE GOING TO BE REPLACED BY A .SO LIBRARY"""

def get_user(token: str):
	# Returns a dictionary with user details
	return {
		"id": 1,
		"name": "user1",
		"money": 100,
		"products": [
			{"product_id": 1, "amount": 4},
			{"product_id": 2, "amount": 8},
			{"product_id": 3, "amount": 2}
		]
	}

def get_product(product_id: int):
	# Returns a dictionary with product details
	return {
		"id": 1,
		"name": "product1",
		"value": 100,
		"amount": 4
	}

def get_products():
	# (id, name, value, amount)
	return [
			{"id": 1, "name": "product1", "value": 100, "amount": 4},
			{"id": 2, "name": "product2", "value": 200, "amount": 8},
			{"id": 3, "name": "product3", "value": 300, "amount": 2}
	]

def buy_product(product_id: int, amount: int):
	return True

def sell_product(product_id: int, amount: int):
	return True
