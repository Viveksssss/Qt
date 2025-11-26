# resources.py
import os
from pathlib import Path


class Resources:
    def __init__(self):
        self.resource_dir = self.find_resource_dir()

    def find_resource_dir(self):
        # 在当前目录和上级目录中查找resource文件夹
        possible_paths = [
            Path(".") / "resource",
            Path(".") / ".." / "resource",
            Path(__file__).parent / "resource",
            Path(__file__).parent.parent / "resource"
        ]

        for path in possible_paths:
            if path.exists() and path.is_dir():
                return str(path.resolve())

        # 如果找不到，创建resource目录
        resource_path = Path(".") / "resource"
        resource_path.mkdir(exist_ok=True)
        return str(resource_path.resolve())

    def get_path(self, filename):
        return str(Path(self.resource_dir) / filename)

    def exists(self, filename):
        return Path(self.resource_dir) / filename


# 创建全局资源管理器
resources = Resources()