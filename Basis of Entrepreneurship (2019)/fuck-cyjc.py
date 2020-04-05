#!/usr/bin/env python3
#
__LOGO__ = \
'''
 _____  _   _   ____  _  __         ____ __   __     _   ____
|  ___|| | | | / ___|| |/ /        / ___|\ \ / /    | | / ___|
| |_   | | | || |    | ' /  _____ | |     \ V /  _  | || |
|  _|  | |_| || |___ | . \ |_____|| |___   | |  | |_| || |___
|_|     \___/  \____||_|\_\        \____|  |_|   \___/  \____| v{!s}
                                                            --By xjasonlyu
'''
#
# 杭电两学分创业基础刷课脚本
# 仅自用并测试过杭电平台，其他平台可用性未经测试
#
# 脚本作者：xjasonlyu
# 脚本地址：https://gist.github.com/xjasonlyu/cc0c3cf3b12930d3a2256d9e1b300e64
#
# 使用方式：
# - python3 fuck-cyjc.py
#
# 1. 可以直接修改main函数中的`account`和`password`参数
# 2. 或者也可以直接运行，在提示中输入参数信息（输入密码时没有显示是正常的）
# 3. 默认是1-8周的章节，可以修改main自定义范围
#

import os
import sys
import json
import time
import pickle
import random
import getpass
import os.path as op
from io import BytesIO
from urllib.parse import urlencode

# Third-party library, installation required
import demjson
import requests
from PIL import Image
from retrying import retry


__VERSION__ = "0.0.3"
__SESSION__ = "session.dat"
__USERINFO__ = "userinfo.dat"


def retry_if_timeout(result):
    # Workaround for some incomplete cases
    return result > 30


def retry_if_not_keyboard_interrupt(exception):
    return not isinstance(exception, KeyboardInterrupt)


class WATCHER:

    commit_time = 30.0
    cookie_file = __SESSION__

    def __init__(self, account, password, school_id, course_list):
        self.account = account
        self.password = password
        self.course_list = course_list

        self.schoolID = school_id
        self.studentID = None

        # watching session
        self.player = requests.Session()
        self.player.trust_env = False
        self.session = requests.Session()
        self.session.trust_env = False

        if op.exists(self.cookie_file):
            # load cookie from file
            with open(self.cookie_file, 'rb') as f:
                self.session.cookies.update(pickle.load(f))
                print(f"Load session from '{__SESSION__}': {self.session.cookies.get_dict()}")

    def save_cookie(self):
        # save cookie to file
        with open(self.cookie_file, 'wb') as f:
            pickle.dump(self.session.cookies, f)

    @retry(retry_on_result=retry_if_timeout, stop_max_attempt_number=5, wait_random_min=1000, wait_random_max=5000)
    def start(self):
        start_time = time.time()  
        for _id in self.course_list:
            # course id
            self.id = _id
            print("@New chapter: {:03d} （第{:d}周）".format(_id, _id-137))

            # login site
            result = self.checkOnline()
            if result != "success":
                print("Cookie expired, try to do-login")
                self.doLogin()
            else:
                print("Already logined!")

            if not self.studentID:
                studentID = self.getStudentID()
                print("Get student ID: {}".format(studentID))
                self.studentID = studentID

            # start watching
            courseList = self.getList()
            if not len(courseList):
                print("courseList is empty!")
                continue

            unfinished_courses = []
            for course in courseList:
                if course['status'] == 'completed':
                    print("course: {id}-{title}-{href}-({status}) ({finishLength} min) is finished".format(**course))
                else:
                    print("course: {id}-{title}-{href}-({status}) [{location}] ({finishLength} min) is unfinished".format(**course))
                    unfinished_courses.append(course)

            for course in unfinished_courses:
                courseID = course['id']

                print("start watching course: {id}-{title}-{href}-({status}) ({finishLength} min)".format(**course))
                sessionID = self.getSessionID(course)
                print("New Session ID: {}".format(sessionID))

                # do open
                self.openSite(sessionID)
                self.openPlayer(sessionID, course)

                # loop
                self.loopCommit(sessionID, course)

                # sleep random seconds
                s = random.uniform(5.0, 30.0)
                print("Script will sleep for {:.2f}s".format(s))
                time.sleep(s)
        return time.time() - start_time

    @staticmethod
    def getCode(data: bytes) -> str:
        f = BytesIO(data)
        i = Image.open(f)
        # display image
        i.show()

        text = input("Input captcha code: ")
        if not text:
            raise Exception("input code error")
        return text

    @staticmethod
    def _referer_1(_id):
        return "http://acadol.cn/LMS/coursePlayer/player.do?id={}".format(_id)

    @staticmethod
    def _referer_2(sessionID):
        return "http://116.62.4.103:8082/coursePlayer/open.do?sessionId={}".format(sessionID)

    def _headers(self):
        return {
            "Content-Type": "application/x-www-form-urlencoded; charset=UTF-8",
            "X-Requested-With": "XMLHttpRequest",
            "Referer": self._referer_1(self.id),
        }

    def doLogin(self):
        url = "http://acadol.cn/LMS/hunan/index.do"

        r = self.session.get(url)
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        # captcha code
        url = "http://acadol.cn/code"
        r = self.session.get(url, headers={"Referer": "http://acadol.cn/LMS/login.do"})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        # get code from input
        _code = self.getCode(r.content)

        url = "http://acadol.cn/LMS/doLogin.do"
        r = self.session.post(url, headers={"Content-Type": "application/x-www-form-urlencoded", "Referer": "http://acadol.cn/LMS/login.do"},
        data={
            "redirectUrl": "",
            "msgType": "",
            # school type: 4 -> HDU
            "companyId": self.schoolID,
            "txt_loginName": self.account,
            "txt_password": self.password,
            "txt_code": _code,
        })

        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        url = "http://acadol.cn/LMS/study/personalv2/index.do"
        r = self.session.get(url, headers={"Referer": "http://acadol.cn/LMS/login.do"})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))
        return

    def checkOnline(self):
        url = "http://acadol.cn/LMS/checkStaffOnline.do"
        r = self.session.post(url, headers=self._headers())
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))
        return r.text

    def getStudentID(self):
        url = "http://acadol.cn/LMS/ajax/site/coursenotes/answer_player.do"
        r = self.session.post(url, headers=self._headers(), data={"courseId": self.id})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        data = json.loads(r.text)
        if type(data) != dict:
            raise TypeError("{} is not dict".format(type(data)))

        studentID = data.get("staffId")
        if not studentID:
            raise Exception("get student id error")
        return studentID

    def getList(self):
        url = "http://acadol.cn/LMS/ajax/coursePlayer/getStaffScoList.do"
        r = self.session.post(url, headers=self._headers(), data={"id": self.id})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        data = json.loads(r.text)
        if type(data) != dict:
            raise TypeError("{} is not dict".format(type(data)))

        # print("DEBUG -> LIST:", data)

        courseStr = data.get("scoList")
        if not courseStr:
            raise Exception("fuck it, get course error")
        return demjson.decode(courseStr)

    def getSessionID(self, course):
        url = "http://acadol.cn/LMS/ajax/coursePlayer/onlineCoursePlayer.do"
        data = {
            "courseNumer": "{:03d}".format(int(self.id)-137),
            "courseType": course['type'],
            "href": course['href'],
            "requiredTime": course['finishLength'],
            "location": None,
            "suspeDate": None, 
            "url": "http://116.62.4.103:8082/coursePlayer/init.do",
            "status": None,
            "totalTime": 0,
            "id": course['chapterId'],
            "staffChapterId": course['id'],
            "courseId": self.id,
        }
        r = self.session.post(url, headers=self._headers(), data=data)
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))
        # server will respond session id
        return r.text

    def openSite(self, sessionID):
        url = "http://116.62.4.103:8082/coursePlayer/open.do"
        payload = { 'sessionId': sessionID }

        r = self.player.get(url, params=payload, headers={"Referer": self._referer_1(self.id)})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))

        _cookies = self.player.cookies.get_dict()
        if _cookies:
            course_yun = _cookies.get('course_yun')
            if course_yun:
                print("New CourseYun cookie set: {}".format(course_yun))
                self.yun_cookie = { "course_yun": course_yun }
        return

    def openPlayer(self, sessionID, course):
        url = "http://116.62.4.103:8082/coursePlayer/player.do"
        payload = {
            "courseType": course['type'],
            "file": "/data/4/{}//{}".format("{:03d}".format(self.id-137), course['href']),
        }
        r = self.player.get(url, params=payload, headers={"Referer": self._referer_2(sessionID)})
        if r.status_code != 200:
            raise Exception("status code ({}) error!".format(r.status_code))
        return

    @retry(retry_on_exception=retry_if_not_keyboard_interrupt, stop_max_attempt_number=5, wait_random_min=10000, wait_random_max=30000)
    def loopCommit(self, sessionID, course):
        url = "http://116.62.4.103:8082/coursePlayer/loopCommit.do"
        commitData = {
            "student_id": self.studentID,
            # offset
            "lesson_location": "", #! must
            "lesson_status": "not attempted",
            "score": "0.0",
            # empty field
            "suspend_data": "",
            "session_time": "00:00:30",
            "lesson_progress": "0.0",
            "masteryscore": "",
            "total_time": "0",
            "required_time": course['finishLength'],
            "session_id": sessionID,
        }

        def doCheck():
            result = self.checkOnline()
            if result != "success":
                raise Exception("online status: {}".format(result))
            print("CHECK ONLINE: {} -> ({:.0f})".format(result, time.time()))

        location = 0.0
        if course['location'] != 'null':
            try:
                location = float(course['location'])
            except:
                pass
        print("START: auto watch from {:.1f}s".format(location))

        videoLength = 60 * float(course['finishLength'])

        if location > videoLength:
            print("DETECTED: location > videoLength, reset location to zero")
            location = 0.0

        random_offset = random.uniform(60.0, 90.0)
        print("ALL WATCH LENGTH: {:.1f}s".format(videoLength + random_offset))

        # loop started
        while location < videoLength + random_offset:
            period = self.commit_time - 1 + random.uniform(1.0, 2.0)

            # sleep for a period
            for _ in range(3):
                time.sleep(period/3)
                doCheck()

            location += period
            if location > videoLength + random_offset:
                location = videoLength + random_offset

            # data
            payload = commitData
            payload["lesson_location"] = "{:.1f}".format(location)

            r = self.player.get(url, params=payload, headers={"X-Requested-With": "XMLHttpRequest", "Referer": self._referer_2(sessionID)})
            # log info
            print("COMMIT ({:03d}): GET '{}?{}'".format(self.id-137, url, urlencode(payload)))
            if r.status_code != 200:
                raise Exception("status code ({}) error!".format(r.status_code))

        # loop ended
        print("DONE: course: {id}-{title}-{href} is finished!".format(**course))
        return


# MAIN FUNCTION
def main():
    # change dir to __file__ path
    p = op.abspath(op.dirname(__file__))
    os.chdir(p)
    # show LOGO
    print(__LOGO__.format(__VERSION__), end='')

    try:
        w = None
        # Range for sections, edit if in need
        # 138: 第一周
        # 145: 第八周
        # default: from 1st to 8th week
        begin_week = 1
        end_week = 8
        ### info:
        user_info = {
            "account": "",
            "password": "",
            # '4' for HDU
            "school_id": 4,
            "course_list": range(begin_week+137, end_week+137+1),
        }
        ###
        #
        # Require user info if data doesn't exists
        if op.exists(__USERINFO__):
            with open(__USERINFO__, 'rb') as f:
                user_info = pickle.load(f)
                print(f"Load userinfo from '{__USERINFO__}' successfully!")
        else:
            if not user_info.get("account"):
                user_info["account"] = input("Account: ")
            if not user_info.get("password"):
                user_info["password"] = getpass.getpass()
            if not user_info.get("school_id"):
                user_info["school_id"] = input("School ID: ")
            # if not user_info.get("course_list"):
            #     begin_week = input("Enter begin week(1): ")
            #     end_week = input("Enter end week(8): ")
            #     user_info["course_list"] = range(begin_week+_offset, end_week+_offset+1)
            with open(__USERINFO__, 'wb') as f:
                pickle.dump(user_info, f)

        w = WATCHER(**user_info)
        w.start()
    except KeyboardInterrupt:
        print("Ctrl+C, exit")
    finally:
        if w:
            w.save_cookie()


if __name__ == "__main__":
    main()
