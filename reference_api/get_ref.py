import requests
import json
from versions import kversion_5, kversion_6

PROJECT = "linux"
URL = f"http://elixir.mrt.red:9090/api/ident/{PROJECT}/"

def get_symbol_references(symbol, version="latest"):

    r = requests.get(URL+symbol, params={"version": version})

    api_res = json.loads(r.text)
    references = api_res.get("references")

    if references == None:
        print("No references found")
        return None
    
    return references

if __name__ == "__main__":
    # cdev?version=latest
    # no family means C
    #kversion_6.go_next()
    version = str(kversion_5)
    #version = "latest"
    symbol = "commit_creds"

    print(get_symbol_references(symbol, version))

