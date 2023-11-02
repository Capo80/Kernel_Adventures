VERSION_6_MAX = 6
VERSION_5_MAX = 20

class KernelVersion:

    def __init__(self, n1, n2, max) -> None:
        self.n1 = n1
        self.n2 = n2
        self.max = max

    def __str__(self) -> str:
        return f"v{self.n1}.{self.n2}"

    def go_next(self) -> bool:
        self.n2 += 1
        if (self.n2 > self.max):
            return False
        else:
            return True
    

kversion_6 = KernelVersion(6, 0, VERSION_6_MAX)
kversion_5 = KernelVersion(5, 2, VERSION_5_MAX)