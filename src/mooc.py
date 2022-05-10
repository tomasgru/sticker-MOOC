import requests
import json
import time
import sys

class mooc():
    def __init__(self):
        jsonDoc = open('moocConfig.json','r')
        self.mob_token = json.load(jsonDoc)['mob_token']
        self.output = {'homeworks' : [], 'quizs' : [], 'evaluate' : [], 'exams' : []}
        self.items = ['homeworks', 'quizs']
        
    def getAllMyCourseList(self):
        url = 'https://www.icourse163.org/mob/course/getAllMyCourseList/v2'
        payload = {
            'mob-token' : self.mob_token,
            'p' : '0',
            'psize' : '20',
            'type' : '30'
        }
        courseList = []
        response = requests.post(url, data=payload).json()
        currentTime = int(time.time()-28800)*1000
        for x in response['results']['result']:
            if x['termPanel']['startTime'] <= currentTime <= x['termPanel']['endTime']:
                courseList.append({
                    'cid' : x['id'],
                    'name' : x['name'],
                    'tid' : x['termPanel']['id']
                })
        fp = open('courseList.json', 'w')
        json.dump(courseList, fp)
        
    def getCourseTaskList(self):
        jsonDoc = open('courseList.json','r',encoding="utf8")
        courseList = json.load(jsonDoc)
        url = 'https://www.icourse163.org/mob/course/courseLearn/v2'
        evaluateUrl = 'https://www.icourse163.org/mob/course/homeworkInfo/v1'
        for x in courseList:
            payload = {
                'cid' : x['cid'],
                'mob-token' : self.mob_token,
                'tid' : x['tid']
            }
            response = requests.post(url, data=payload).json()
            currentTime = response['extraInfo']['currentTime']
            for y in response['results']['termDto']['chapters']:
                for it in self.items:
                    try:
                        for z in y[it]:
                            if z['test']['deadline'] >= currentTime and (not z['test']['usedTryCount']):
                                deadline = z['test']['deadline']//1000
                                self.output[it].append({
                                    'id' : z['test']['id'],
                                    'name' : z['test']['name'],
                                    'deadline' : deadline - (60 if (deadline+28800) % 86400 == 0 else 0),
                                    'belongName' : x['name'],
                                    'belongId' : x['cid']
                                })
                            if it == 'quizs': continue
                            if z['test']['evaluateStart'] <= currentTime <= z['test']['evaluateEnd']:
                                contentId = z['contentId']
                                res = requests.post(evaluateUrl, data={
                                    'mob-token' : self.mob_token,
                                    'tid' : contentId
                                }).json()['results']['mocHomeworkEvalDto']
                                if res['complatedEvalCount'] < res['evalTotalCount']:
                                    deadline = z['test']['evaluateEnd']//1000
                                    self.output['evaluate'].append({
                                        'id' : z['test']['id'],
                                        'name' : z['test']['name'],
                                        'deadline' : deadline - (60 if (deadline+28800) % 86400 == 0 else 0),
                                        'belongName' : x['name'],
                                        'belongId' : x['cid']
                                    })
                    except: pass
            try:
                for y in response['results']['termDto']['exams']:
                    if y['deadline'] >= currentTime and (not y['usedTryCount']):
                        deadline = y['deadline']//1000
                        self.output['exams'].append({
                            'id' : y['id'],
                            'name' : y['name'],
                            'deadline' : deadline - (60 if (deadline+28800) % 86400 == 0 else 0),
                            'belongName' : x['name'],
                            'belongId' : x['cid']
                        })
            except: pass
        fp = open('taskList.json', 'w')
        json.dump(self.output, fp)
        
if __name__ == '__main__':
    a = mooc()
    try:
        if (sys.argv[1]=="getAllMyCourseList"):
            a.getAllMyCourseList()
        elif (sys.argv[1]=="getCourseTaskList"):
            a.getCourseTaskList()
    except: pass
