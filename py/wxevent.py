from wxauto import *
import time
import sys
import os
import json
import datetime
import base64
import re
import requests
from openai import OpenAI
import argparse

parser = argparse.ArgumentParser(description='微信监听')
parser.add_argument('--api_key', type=str, required=True, help='硅基流动API密钥')
parser.add_argument('--dify_key', type=str, default="", help='Dify API密钥')
parser.add_argument('--send_greeting', type=int, choices=[0, 1], default=1, help='监听时是否发送检测信息')
parser.add_argument('--save_pic', type=int, choices=[0, 1], default=1, help='是否保存图片')
parser.add_argument('--save_file', type=int, choices=[0, 1], default=1, help='是否保存文件')
parser.add_argument('--save_voice', type=int, choices=[0, 1], default=1, help='是否保存语音')
parser.add_argument('--contacts', nargs='+', required=True, help='监听的联系人列表')
args = parser.parse_args()


listen_list = args.contacts
api_key = args.api_key
dify_key = args.dify_key
send_greeting = bool(args.send_greeting)
save_pic = bool(args.save_pic)
save_file = bool(args.save_file)
save_voice = bool(args.save_voice)

# 打印参数信息
print(f"[Python] 接收到的参数:")
print(f"  API密钥: {api_key}")
print(f"  Dify密钥: {dify_key}")
print(f"  发送检测信息: {send_greeting}")
print(f"  保存图片: {save_pic}")
print(f"  保存文件: {save_file}")
print(f"  保存语音: {save_voice}")
print(f"  监听列表: {listen_list}")


# 获取当前微信客户端
wx = WeChat()

# 获取会话列表
wx.GetSessionList()

# 向某人发送消息（以`文件传输助手`为例）
# msg = '你好~'
# who = '文件传输助手'
# wx.SendMsg(msg, who)  # 向`文件传输助手`发送消息：你好~


# 添加监听
for i in listen_list:
    wx.AddListenChat(who=i, savepic=save_pic, savefile=save_file, savevoice=save_voice)
    wx.SendMsg("你好呀，我是严小希，很高兴认识你~", i)

# 各种api
# LLM
client = OpenAI(
    api_key=api_key,
    base_url="https://api.siliconflow.cn/v1",
)
# Dify
# 配置区
API_KEY = dify_key
USER = "mhx_tql_tql"
DIFY_SERVER = "https://api.dify.ai"  # 你的Dify服务器地址
WORKFLOW_API_KEY = API_KEY  # 如果工作流和上传用同一个key


def stringIn(s):
    # ===================================================
    # 处理消息逻辑

    system_prompt = """
    用户将提供一些日程信息。请解析其中的“开始时间”,“结束时间”,“日程事件”和“链接地址”,其中未提及的信息请返回"NULL",开始时间和结束时间精确到分钟（向上取整）,返回重要性（0-4间的整数，0为最重要，4为最不重要），返回事件描述（阐述事件重点和重要信息）。并以JSON格式输出。

    示例输入：
    以上为国关国组专业宣讲会时间（2025年4月11日 04：30-16：00），欢迎感兴趣同学参加。

    示例JSON输出：
    {
        "year": "2025",
        "month": "4",
        "day": "11",
        "start_time": "04:30",
        "end_time": "16:00",
        "event": "国关国组专业宣讲会",
        "importance":"1",
        "url": "NULL",
        "description": "国关国组专业宣讲会，欢迎感兴趣同学参加。"
    }
    """
    user_prompt = "今天是"
    current_datetime = datetime.date.today()
    # 时间要转换成标准字符串
    user_prompt += current_datetime.strftime('%Y-%m-%d')
    week_list = ["星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"]
    user_prompt += week_list[datetime.date.today().weekday()]
    user_prompt += " "
    # 输入事件信息
    user_prompt += s

    messages = [{"role": "system", "content": system_prompt},
                {"role": "user", "content": user_prompt}]

    print("[server][python]已向远程服务器提交日程原文")

    # 尝试提交
    max_retries = 3
    for attempt in range(max_retries):
        try:
            response = client.chat.completions.create(
                model="deepseek-ai/DeepSeek-V3",
                messages=messages,
                response_format={'type': 'json_object'}
            )
            return response
        except Exception as e:
            if attempt < max_retries - 1:
                print(f"请求失败，正在重试 ({attempt+1}/{max_retries})...")
                time.sleep(2)  # 等待2秒后重试
            else:
                raise e


def image_to_base64(image_path):
    with open(image_path, "rb") as image_file:
        encoded_string = base64.b64encode(image_file.read()).decode('utf-8')
        return f"data:image/{image_path.split('.')[-1]};base64,{encoded_string}"


def PictureIn(pic):
    # ===================================================
    # 处理图片逻辑
    system_prompt = """
    你是一个助手，负责从用户提供的图片中提取日程信息。用户会提供一张包含日程信息的图片，
    你需要识别图片中的文字，并从中提取出日程信息，包括日期、开始时间、结束时间、活动内容等。
    """
    user_prompt = "今天是"
    current_datetime = datetime.date.today()
    # 时间要转换成标准字符串
    user_prompt += current_datetime.strftime('%Y-%m-%d')
    week_list = ["星期一", "星期二", "星期三", "星期四", "星期五", "星期六", "星期日"]
    user_prompt += week_list[datetime.date.today().weekday()]
    user_prompt += "。 请解析上面这张图片中包含的日程信息（日期、开始时间、结束时间、活动内容、网址、活动描述、活动标题等等）"
    # 输入事件信息
    messages = [{"role": "system", "content": system_prompt},
                {"role": "user", "content": [{"type": "text", "text": user_prompt},
                                             {"type": "image_url", "image_url": {"url": image_to_base64(pic)}}]}]

    print("[server][python]已向远程服务器提交日程原图")

    # 尝试提交
    max_retries = 3
    for attempt in range(max_retries):
        try:
            response = client.chat.completions.create(
                model="Qwen/Qwen2.5-VL-32B-Instruct",
                messages=messages,
            )
            return response
        except Exception as e:
            if attempt < max_retries - 1:
                print(f"请求失败，正在重试 ({attempt+1}/{max_retries})...")
                time.sleep(2)  # 等待2秒后重试
            else:
                raise e

    return response


# dify文档上传
def upload_file(local_file_path, api_key):
    file_type = 'application/octet-stream'  # 可根据实际类型调整
    url = f'{DIFY_SERVER}/files/upload'
    headers = {'Authorization': f'Bearer {api_key}'}
    with open(local_file_path, 'rb') as file:
        files = {'file': (os.path.basename(local_file_path), file, file_type)}
        data = {'user': USER}
        response = requests.post(url, headers=headers, files=files, data=data)
    if response.status_code == 201:
        print(f"[server][python][文档上传成功] {local_file_path}")
        return response.json()['id']
    else:
        print(f"[server][python][文档上传失败] {local_file_path}，状态码: {response.status_code}")
        return None


# 文档解析
def DocumentIn(file_path):
    print(f"[server][python]开始处理文档: {file_path}")

    file_id = upload_file(file_path,API_KEY)

    workflow_url = f"{DIFY_SERVER}/v1/workflows/run"
    headers = {
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json"
    }
    data = {
        "inputs": {
            "upload": {
                "transfer_method": "local_file",
                "upload_file_id": file_id,
                "type": "document"
            }
        },
        "response_mode": "blocking",
        "user": USER
    }
    try:
        response = requests.post(workflow_url, headers=headers, json=data)
        if response.status_code == 200:
            print(f"[server][python][dify工作流执行成功] 文件ID: {file_id}")
            return response.json(), True
        else:
            print(f"[server][python][dify工作流执行失败] 文件ID: {file_id}，状态码: {response.status_code}")
            return {"status": "error", "message": f"Failed to execute workflow, status code: {response.status_code}"}, False
    except Exception as e:
        print(f"[server][python][dify工作流异常] 文件ID: {file_id}，错误: {str(e)}")
        return {"status": "error", "message": str(e)}, False


# 文件类型识别
def check_file_type(file_path):
    # 可用图片扩展名
    image_extensions = ['.jpg', '.jpeg', '.png',
                        '.gif', '.bmp', '.tiff', '.webp']
    # 可用文档扩展名
    document_extensions = ['txt', '.markdown', '.mdx', '.pdf', '.html', '.xlsx', '.xls', '.vtt',
                           '.properties', '.doc', '.docx', '.csv', '.eml', '.msg', '.pptx', '.xml', '.epub', '.ppt', '.md', '.htm']

    # 获取文件扩展名
    _, ext = os.path.splitext(file_path)
    ext = ext.lower()  # 转换为小写

    if ext in image_extensions:
        return "图片"
    elif ext in document_extensions:
        return "文档"
    else:
        return "未知类型"


# 持续监听消息，并且收到消息后回复“收到”
wait = 1  # 设置1秒查看一次是否有新消息
while True:
    msgs = wx.GetListenMessage()
    for chat in msgs:
        who = chat.who              # 获取聊天窗口名（人或群名）
        one_msgs = msgs.get(chat)   # 获取消息内容
        # 消息检查
        for msg in one_msgs:
            msgtype = msg.type       # 获取消息类型
            content = msg.content    # 获取消息内容，字符串类型的消息内容

            if msgtype == 'friend':
                sys.stdout.flush()  # 中断之前的输出
                print(f'[WeChatMsg][{who}]: {content}')
                if (send_greeting):
                    chat.SendMsg('小希收到，正在为您处理~')
                temp = "文本"
                if (os.path.exists(content)):
                    path_temp = content
                    temp = check_file_type(content)
                    if (temp == "图片"):
                        content = PictureIn(content)
                        content = content.choices[0].message.content
                        print("[server][python]图片解析完成，尝试解析日程")
                        parsed_response = json.loads(
                            stringIn(content).choices[0].message.content)
                    elif (temp == "文档"):
                        # parsed_response = DocumentIn(content)
                        print("[server][python]文件解析服务暂不可用，请尝试使用文本、图片或语音发送")
                        if (send_greeting):
                            chat.SendMsg('文件解析服务暂不可用，请尝试使用文本或图片发送')
                        continue
                    else:
                        if (send_greeting):
                            print("[server][python]不支持的信息类型")
                        chat.SendMsg('暂不支持此类信息，请尝试发送其他格式消息')
                        continue
                else:
                    print("[server][python]处理普通文本")
                    parsed_response = json.loads(
                        stringIn(content).choices[0].message.content)
                parsed_response["fromwho"] = who
                if (temp == "图片" or temp == "文档"):
                    parsed_response["docu"] = path_temp
                else:
                    parsed_response["docu"] = "NULL"

                print("[server][python]日程解析信息", parsed_response)

                # 向C++qt发送信息
                sys.stdout.write("QT_DATA_START:" +
                                 json.dumps(parsed_response) + "QT_DATA_END")
                sys.stdout.flush()  # 立即输出
                # 消息处理逻辑结束
                # ===================================================
                if (send_greeting):
                    chat.SendMsg(
                        "日程解析成功：\n"+json.dumps(parsed_response, ensure_ascii=False, indent=4))

    time.sleep(wait)
