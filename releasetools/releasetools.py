# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import common

def FullOTA_InstallEnd(info):
    info.script.Mount("/bootdata")
    info.script.AppendExtra('package_extract_file("boot.img", "/bootdata/boot.img");')
    info.script.AppendExtra('package_extract_file("install/bootdata/config.ini", "/bootdata/config.ini");')
    info.script.AppendExtra('package_extract_dir("install/bootdata/dtbs", "/bootdata/dtbs");')
    info.script.Unmount("/bootdata")
