

test1 = ["a", "b", "c"]

def test2(test):
    for el in test:
        if el == "b":
            test.remove(el)
            return el

print(test2(test1))