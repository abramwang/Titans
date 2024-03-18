import re
import requests
from lxml import html

# 检查输入字符串是不是一个HTTP URL或者Linux路径

def check_string(input_string):
    # 正则表达式模式匹配HTTP URL
    http_pattern = r'^https?://[a-zA-Z0-9.-]*$'
    
    # 正则表达式模式匹配Linux路径
    linux_path_pattern = r'^/([a-zA-Z0-9.-/]*)*$'
    
    # 匹配HTTP URL
    if re.match(http_pattern, input_string):
        print(f"{input_string} 是一个HTTP URL")
        return "URL"
    # 匹配Linux路径
    elif re.match(linux_path_pattern, input_string):
        print(f"{input_string} 是一个Linux路径")
        return "Path"
    else:
        print(f"{input_string} 不是一个HTTP URL也不是一个Linux路径")
        return "Unknown"

# 获取nginx服务器上的文件列表
def get_nginx_file_path_list(url):
    response = requests.get(url)
    # 检查请求是否成功
    if response.status_code == 200:
        # 打印网页内容
        html_content = response.text
        return html_content
    else:
        print(f"Failed to retrieve HTML content. Status code: {response.status_code}")
        return None

# 解析html文件列表，返回所有的parquet文件
def pares_file_list(html_doc):
    html_tree = html.fromstring(html_doc)

    # 获取所有列表项的文本
    list_items = html_tree.xpath('.//a/text()')
    
    file_list = []
    for item in list_items:
        if ".parquet" in item:
            file_list.append(item)
    return file_list

# 获取路径下的所有文件列表
def get_file_list_in_path(url):
    html_doc = get_nginx_file_path_list(url)
    file_list = pares_file_list(html_doc)
    return file_list