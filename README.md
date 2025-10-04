# AndroLua SO 文件自动编译与提取

![GitHub Actions Workflow Status](https://github.com/cinitdev/AndroluaSO_Extract/actions/workflows/build_and_send.yml/badge.svg)

本项目提供了一个全自动化的 GitHub Actions 工作流，用于编译 AndroLua Pro 安卓项目，并从中提取指定的 `.so` 动态库文件。编译完成后，工作流会自动将打包好的 `.so` 文件发送到指定的 Telegram 聊天，并附带详细的编译报告。

这个项目是为那些希望在云端自动构建和获取最新 `.so` 文件的开发者设计的。

---

## ✨ 功能特性

* **☁️ 云端自动编译**: 无需配置本地安卓开发环境，每次提交代码或手动触发即可在 GitHub 云端服务器上完成编译。
* **📦 两种提取模式**: 手动触发时，可以精确指定需要提取的一个或多个 `.so` 文件；如果留空，则会自动提取 APK 中所有的 `.so` 文件。
* **🤖 智能 Telegram 通知**: 使用 **Pyrogram** 发送媒体消息，将编译产物（ZIP压缩包）和详细的编译信息整合在一条消息中，界面美观且信息清晰。
* **⚠️ 失败报告**: 当你按需提取某个 `.so` 文件但它在编译产物（APK）中不存在时，最终的通知消息会明确指出具体是哪个文件提取失败了。
* **🚀 双重触发机制**:
    * **手动触发**: 可以在 GitHub Actions 页面手动运行，并传入自定义参数。
    * **自动触发**: 当有代码 `push` 到 `master` 分支时，会自动提取全部 `.so` 文件。

## 🚀 快速开始

如果你 Fork 了本项目，你需要完成以下设置才能让 GitHub Actions 正常工作。

### 第一步: 获取 Telegram 必要凭证

工作流需要通过 Telegram 的官方 API 和机器人 API 来发送消息，因此你需要准备以下四样东西：

1.  **`API_ID`** 和 **`API_HASH`**
    * 访问 **[my.telegram.org](https://my.telegram.org)** 并登录你的 Telegram 账号。
    * 点击 "API development tools"。
    * 填写一个应用名称（任意填写，如 `MyWorkflowBot`），然后你就能得到 `api_id` 和 `api_hash`。

2.  **`BOT_TOKEN`** (机器人令牌)
    * 在 Telegram 中搜索 **[@BotFather](https://t.me/BotFather)** 并与他开始对话。
    * 发送 `/newbot` 命令来创建一个新的机器人。
    * 按照提示为你的机器人命名后，BotFather 会给你一长串的 Token，这就是你的 `BOT_TOKEN`。
    * **重要**: 创建好机器人后，记得点击 "Start" 启动一下你的机器人。

3.  **`CHAT_ID`** (聊天 ID)
    * 这是你希望机器人发送消息的目标地。它可以是你的个人账号、群组或频道的 ID。
    * **获取方法**: 在 Telegram 中搜索 **[@userinfobot](https://t.me/userinfobot)**，与他开始对话，它会立即返回你的个人账号 ID，即为 `CHAT_ID`。
    * **注意**: 如果你想让机器人发送消息到群组，你需要先将机器人添加到群组中。

### 第二步: 在你的 GitHub 仓库中设置 Secrets

为了安全地存储你的凭证，你需要将它们添加到你的 GitHub 仓库的 Secrets 中。

1.  进入你 Fork 后的 GitHub 仓库页面。
2.  点击右上角的 **Settings** -> 左侧菜单栏的 **Secrets and variables** -> **Actions**。
3.  点击 **New repository secret** 按钮，逐一添加以下 **4** 个 Secret：

| Secret 名称          | 你的值                                       |
| -------------------- | -------------------------------------------- |
| `PYROGRAM_API_ID`    | 填入你从 Telegram 网站获取的 `api_id`          |
| `PYROGRAM_API_HASH`  | 填入你从 Telegram 网站获取的 `api_hash`        |
| `TELEGRAM_BOT_TOKEN` | 填入你从 [@BotFather](https://t.me/BotFather) 获取的机器人 `Token`         |
| `TELEGRAM_CHAT_ID`   | 填入你从 [@userinfobot](https://t.me/userinfobot) 获取的 `CHAT_ID`         |

**完成以上设置后，你的项目就可以正常运行了！**

---

## 🕹️ 如何使用

### 方式一：手动触发 (推荐)

这是最灵活的使用方式，可以自定义提取的 SO 文件。

1.  进入你的仓库主页，点击上方的 **Actions** 标签页。
2.  在左侧列表中，点击 **Build AndroLua Pro SO and Send to Telegram**。
3.  在右侧，你会看到一个 "This workflow has a workflow_dispatch event" 的提示，点击 **Run workflow** 下拉按钮。
4.  在 **`输入要提取的SO文件名(用|隔开)，留空则提取全部`** 输入框中，根据你的需求操作：
    * **提取全部 .so 文件**: 将输入框**保持为空**，直接点击绿色的 "Run workflow" 按钮。
    * **提取指定 .so 文件**: 填入文件名。例如：
        * 提取单个: `libluajava.so`
        * 提取多个: `libluajava.so|liblfs.so|libsqlite3.so` (文件名之间用 `|` 分隔)
5.  点击 **Run workflow** 按钮，等待任务完成即可在 Telegram 收到消息。

### 方式二：自动触发

当你向 `master` 分支 `push` 任何代码时，工作流会自动运行。在这种情况下，它会**自动提取 APK 中所有的 .so 文件**。

---

## 📁 项目文件结构

* `/.github/workflows/build_and_send.yml`: GitHub Actions 的核心配置文件，定义了所有的编译和自动化步骤。
* `/send_telegram_message.py`: 使用 Pyrogram 库编写的 Python 脚本，专门负责构造消息内容并将其发送到 Telegram。
