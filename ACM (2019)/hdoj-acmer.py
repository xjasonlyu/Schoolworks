#!/usr/bin/env python3
#
__VERSION__ = "0.0.1"
__LOGO__ = f"""
 _   _  ____    ___       _            _      ____  __  __
| | | ||  _ \  / _ \     | |          / \    / ___||  \/  |  ___  _ __
| |_| || | | || | | | _  | | _____   / _ \  | |    | |\/| | / _ \| '__|
|  _  || |_| || |_| || |_| ||_____| / ___ \ | |___ | |  | ||  __/| |
|_| |_||____/  \___/  \___/        /_/   \_\ \____||_|  |_| \___||_|  v{__VERSION__}
                                                                    --By xjasonlyu
"""

import os
import re
import html
import time
import pickle
import random
import os.path as op

import requests
from bs4 import BeautifulSoup
from retrying import retry


def retry_if_not_keyboard_interrupt(exception):
    return not isinstance(exception, KeyboardInterrupt)


class ACMer:

    lang = {
        "G++": 0,
        "GCC": 1,
        "C++": 2,
        "C": 3,
        "Pascal": 4,
        "Java": 5,
        "C#": 6
    }

    def __init__(self, account, password):
        self.account = account
        self.password = password
        self.session = requests.Session()
        self.session.trust_env = False
        self.cookie_file = f"{account}.dat"

        if op.exists(self.cookie_file):
            # load cookie from file
            with open(self.cookie_file, 'rb') as f:
                self.session.cookies.update(pickle.load(f))
                print(f"Load session from '{self.cookie_file}': {self.session.cookies.get_dict()}")
        if not self._isLoggedIn():
            self._doLogin()

    def save_cookie(self):
        # save cookie to file
        with open(self.cookie_file, 'wb') as f:
            pickle.dump(self.session.cookies, f)

    def _getUserStatus(self):
        url = "http://acm.hdu.edu.cn/userstatus.php"
        r = self.session.get(url, params={"user": self.account})
        if r.status_code != 200:
            raise Exception(f"status code ({r.status_code}) error!")
        return r.text

    def _isLoggedIn(self):
        url = "http://acm.hdu.edu.cn/control_panel.php"
        r = self.session.get(url, allow_redirects=False)
        if not r.status_code in (200, 302):
            raise Exception(f"status code ({r.status_code}) error!")
        loc = r.headers.get("location")
        if loc and 'userloginex.php' in loc:
            print(f"WARNING: user '{self.account}' didn't logged in")
            return False
        elif loc:
            print(f"DEBUG: get header 'Location: {loc}'")
        return True

    def _doLogin(self):
        _url = "http://acm.hdu.edu.cn/"
        r = self.session.get(_url)
        if r.status_code != 200:
            raise Exception(f"status code ({r.status_code}) error!")

        print(f"DEBUG: login to HDOJ as {self.account}")
        url = "http://acm.hdu.edu.cn/userloginex.php"
        r = self.session.post(url, params={"action": "login"}, headers={"Content-Type": "application/x-www-form-urlencoded"},
        data={
            "username": self.account,
            "userpass": self.password,
            "login": "Sign+In"
        })
        if not r.status_code in (200, 302):
            raise Exception(f"status code ({r.status_code}) error!")
        if not self._isLoggedIn():
            raise Exception(f"ERROR: user '{self.account}' login to HDOJ failed")
        return

    def getSolvedPID(self):
        html = self._getUserStatus()
        soup = BeautifulSoup(html, "html.parser")
        p = soup.find_all('p', attrs={"align":"left"})
        j = p[0]
        c = j.find('script')
        r = re.compile(r"p\((\d+?),\d+?,\d+?\);")
        l = r.findall(str(c))
        return tuple(l)

    @retry(retry_on_exception=retry_if_not_keyboard_interrupt, stop_max_attempt_number=2, wait_random_min=10000, wait_random_max=30000)
    def getCode(self, pid):
        url = "http://acm.hdu.edu.cn/status.php"
        payload = {
            "user": self.account,
            "pid": pid,
            "lang": 0,
            "status": 0
        }
        r = self.session.get(url, params=payload)
        if r.status_code != 200:
            raise Exception(f"status code ({r.status_code}) error!")

        soup = BeautifulSoup(r.text, "html.parser")
        p = soup.find_all('tr', attrs={"align": "center"})
        t = None
        for i in p:
            if i.find('font') and i.find('td', attrs={"class": "fixedsize"}):
                t = i
                break
        if not t:
            raise Exception("WANING: nothing find in status")
        # for i in t.find_all('td'):
        #     try:
        #         x = i.contents[0].contents[0]
        #         l.append(x)
        #     except AttributeError:
        #         l.append(i.contents[0])
        l = []
        try:
            for i in t.find_all('td'):
                l.append(i.contents[0])
            for i in (2,3,6,8):
                l[i] = l[i].contents[0]
        except AttributeError as e:
            # probably auth expired
            print("ERROR OCCURRED, RETRYING (RE-LOGIN)")
            if not self._isLoggedIn():
                self._doLogin()
            raise e

        ###
        # e.g. ['11111111', '2019-11-11 11:11:11', 'Accepted', '1111', '11MS', '1111K', '11 B', 'G++', 'name']
        ###
        rid = l[0]
        url = "http://acm.hdu.edu.cn/viewcode.php"
        r = self.session.get(url, params={"rid": rid})
        if r.status_code != 200:
            raise Exception(f"status code ({r.status_code}) error!")

        soup = BeautifulSoup(r.text, "html.parser")
        p = soup.find_all('textarea', attrs={"id": "usercode"})

        assert(len(p) == 1) # if happend, try re-login
        return (tuple(l), html.unescape(str(p[0].contents[0])))

    def submitCode(self, info, code):
        # make sure logged in
        if not self._isLoggedIn():
            print("DEBUG: Login before submit code")
            self._doLogin()

        pid = info[3]
        url = "http://acm.hdu.edu.cn/submit.php"
        r = self.session.get(url, params={"pid": pid})
        if r.status_code != 200:
            raise Exception(f"status code ({r.status_code}) error!")

        url = "http://acm.hdu.edu.cn/submit.php"
        r = self.session.post(url, params={"action": "submit"}, headers={"Content-Type": "application/x-www-form-urlencoded"}, 
        data={
            "check": 0,
            "problemid": pid,
            "language": self.lang[info[-2]],
            "usercode": code
        })
        if not r.status_code in (200, 302):
            raise Exception(f"status code ({r.status_code}) error!")


class FakeACMer:

    extensions = {
        ".c": "GCC",
        ".cpp": "G++",
        ".java": "Java"
        }

    def __init__(self, path):
        self.path = path
        self.info = dict()
        self.account = 'FAKER'

    def getSolvedPID(self):
        l = os.listdir(self.path)
        for i in l:
            n, e = op.splitext(i)
            if e in self.extensions:
                self.info[n] = (i, e)
        return tuple((i for i in self.info))

    def getCode(self, pid: str):
        info = self.info.get(pid)
        if not info:
            return (), ''
        with open(op.join(self.path, info[0]), 'r') as f:
            return ('', '', '', pid, '', '', '', 'G++', ''), f.read()

    def submitCode(self, info, code):
        '''DO NOTHING'''
        pass


def COPY(src, dst, n=-1):
    src_s = set(src.getSolvedPID())
    dst_s = set(dst.getSolvedPID())
    _diff = list(src_s - dst_s)
    _diff.sort()
    if n > 0:
        _diff = _diff[:n]
    if len(_diff):
        print(f"ALL: {_diff}")
    else:
        print("YOU'RE ALL DONE!")

    # do COPY
    i = 0
    for pid in _diff:
        i += 1
        if n > 0 and i > n:
            print("YOU'RE ALL DONE!")
            return
        print(f"COPYING: {src.account}:{pid} => {dst.account}:{pid}")
        t, code = src.getCode(pid)
        print(f"DEBUG: {t}")
        time.sleep(1)
        dst.submitCode(t, code)
        time.sleep(10)
        t, _ = dst.getCode(pid)
        # console
        if t[2].lower() == "accepted":
            print(f"[+]SUCCESS: {t}")
        else:
            print(f"[-]FAILED: {t}")
        # sleep
        if i != len(_diff):
            _t = random.randint(60*1, 60*10)
            print(f"SLEEPING: about {_t//60} min ({len(_diff)-i} more problems left)")
            time.sleep(_t)


def main():
    # change dir to __file__ path
    p = op.abspath(op.dirname(__file__))
    os.chdir(p)
    # show LOGO
    print(__LOGO__, end='')

    try:
        ACM = []
        ACM.append(ACMer("", ""))
        ACM.append(ACMer("", ""))

        assert(len(ACM) == 2)

        COPY(ACM[0], ACM[1])
    except KeyboardInterrupt:
        print("Ctrl+C, exit")
    except Exception as e:
        import traceback
        traceback.print_exc(e)
        import sys
        sys.exit(1)
    finally:
        for a in ACM:
            if a and hasattr(a, 'save_cookie'):
                a.save_cookie()


if __name__ == "__main__":
    main()
