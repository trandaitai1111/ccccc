import cv2
import numpy as np
import requests
import json
import time

# Thiết lập webhook Discord
DISCORD_WEBHOOK_URL = "https://discordapp.com/api/webhooks/1337795837014835200/OxtRiZPOz6be70kB3qeWB5xfL3-B1XzARsRnCLijSVp2fAVkVIJHN9Pf-53Dz5twHzBz"

# Hình ảnh đảo bí ẩn mẫu
REFERENCE_IMAGE_URL = "https://media.discordapp.net/attachments/1332989626843140107/1337796674747371622/Mirage_Clear_View.webp?ex=67a8bfbe&is=67a76e3e&hm=184f25c2c16c9c23d84db327c7e6e66a29f6d1ee631579be9c6f731320d178e9&=&format=webp&width=728&height=450"

# Tải ảnh mẫu về
response = requests.get(REFERENCE_IMAGE_URL, stream=True)
reference_image = None
if response.status_code == 200:
    reference_image = cv2.imdecode(np.frombuffer(response.content, np.uint8), cv2.IMREAD_COLOR)

# Hàm so sánh hình ảnh
def compare_images(server_image_url):
    response = requests.get(server_image_url, stream=True)
    if response.status_code != 200:
        return False
    
    server_image = cv2.imdecode(np.frombuffer(response.content, np.uint8), cv2.IMREAD_COLOR)
    if reference_image is None or server_image is None:
        return False
    
    # Chuyển đổi ảnh sang grayscale
    ref_gray = cv2.cvtColor(reference_image, cv2.COLOR_BGR2GRAY)
    server_gray = cv2.cvtColor(server_image, cv2.COLOR_BGR2GRAY)
    
    # So sánh histogram
    hist_ref = cv2.calcHist([ref_gray], [0], None, [256], [0, 256])
    hist_server = cv2.calcHist([server_gray], [0], None, [256], [0, 256])
    
    similarity = cv2.compareHist(hist_ref, hist_server, cv2.HISTCMP_CORREL)
    return similarity > 0.9  # Ngưỡng tương đồng 90%

# Hàm gửi thông báo lên Discord
def send_discord_notification(server_name, player_count, moon_phase, join_code):
    script_code = f"game:GetService(\"TeleportService\"):TeleportToPlaceInstance(\"PLACE_ID\", \"{join_code}\", game.Players.LocalPlayer)"
    
    embed = {
        "title": "🚨 Đảo bí ẩn đã xuất hiện! 🚨",
        "color": 16711680,
        "fields": [
            {"name": "━━━━━━━━━━━━━━", "value": "\u200b", "inline": False},
            {"name": "📌 **Tên Server**", "value": f"`{server_name}`", "inline": False},
            {"name": "👥 **Số Người Chơi**", "value": f"`{player_count}`", "inline": False},
            {"name": "🌙 **Moon Phase**", "value": f"`{moon_phase}`", "inline": False},
            {"name": "🔑 **Code Join**", "value": f"```{join_code}```", "inline": False},
            {"name": "📜 **Script Join**", "value": f"```lua\n{script_code}\n```", "inline": False},
            {"name": "━━━━━━━━━━━━━━", "value": "\u200b", "inline": False},
        ]
    }
    
    data = {"embeds": [embed]}
    requests.post(DISCORD_WEBHOOK_URL, json=data)

# Giả lập danh sách server để quét
servers = [
    {"name": "Server 1", "player_count": 12, "moon_phase": "Full Moon", "server_image_url": "SERVER_IMAGE_URL_1", "join_code": "12345"},
    {"name": "Server 2", "player_count": 20, "moon_phase": "Half Moon", "server_image_url": "SERVER_IMAGE_URL_2", "join_code": "67890"},
]

# Quét từng server
while True:
    for server in servers:
        if compare_images(server["server_image_url"]):
            send_discord_notification(server["name"], server["player_count"], server["moon_phase"], server["join_code"])
        time.sleep(4)  # Đợi 4 giây trước khi thông báo server tiếp theo
