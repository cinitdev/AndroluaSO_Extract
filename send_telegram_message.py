import os
import asyncio
from pyrogram import Client
from pyrogram.types import InputMediaDocument

async def main():
    """
    使用 Pyrogram 发送带说明文字的文件到 Telegram.
    从环境变量中读取所有必要的参数.
    """
    # --- 从环境变量中获取参数 ---
    api_id = os.getenv("PYROGRAM_API_ID")
    api_hash = os.getenv("PYROGRAM_API_HASH")
    bot_token = os.getenv("PYROGRAM_BOT_TOKEN")
    chat_id = os.getenv("TELEGRAM_CHAT_ID")
    
    file_path = os.getenv("FILE_PATH")
    repo_name = os.getenv("REPO_NAME")
    requested_files = os.getenv("REQUESTED_FILES")
    failed_files = os.getenv("FAILED_FILES", "") # 失败文件列表可能为空

    # --- 检查必要的参数是否存在 ---
    if not all([api_id, api_hash, bot_token, chat_id, file_path, repo_name, requested_files]):
        print("错误：一个或多个必要的环境变量缺失！")
        return

    # --- 构造说明文字 (Caption) ---
    caption_lines = [
        "<b>AndroLua Pro SO 编译完成！</b>",
        "", # 空行
        f"<b>项目:</b> <code>{repo_name}</code>",
        f"<b>提取文件:</b> <code>{requested_files}</code>"
    ]
    
    # 只有在 FAILED_FILES 变量不为空时，才添加“提取失败”这一行
    if failed_files:
        caption_lines.append(f"<b>提取失败:</b> <code>{failed_files}</code>")
        
    final_caption = "\n".join(caption_lines)

    # --- 使用 Pyrogram 发送消息 ---
    # Pyrogram 会话将保存在内存中，不会创建 session 文件
    app = Client("github_actions_bot", api_id=api_id, api_hash=api_hash, bot_token=bot_token, in_memory=True)
    
    try:
        await app.start()
        print(f"正在发送文件 '{file_path}' 到聊天 '{chat_id}'...")
        
        # 发送带说明文字的文档
        await app.send_document(
            chat_id=int(chat_id),
            document=file_path,
            caption=final_caption
        )
        
        print("文件发送成功！")
    except Exception as e:
        print(f"发送消息时出错: {e}")
    finally:
        await app.stop()

if __name__ == "__main__":
    asyncio.run(main())