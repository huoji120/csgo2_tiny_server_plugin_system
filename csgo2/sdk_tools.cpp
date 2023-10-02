#include "sdk_tools.h"

namespace SdkTools {
	auto ProcessChatString(const std::string& input) -> std::tuple<bool, _ChatType, std::string>
	{
        _ChatType chatType;
        std::string content;
        bool success = true;
        // ��������Ƿ��� "say_team" �� "say" ��ͷ
        if (input.size() >= 9 && input.substr(0, 9) == "say_team ") {
            chatType = _ChatType::kTeam;
            content = input.substr(9);
        }
        else if (input.size() >= 5 && input.substr(0, 4) == "say ") {
            chatType = _ChatType::kAll;
            content = input.substr(4);
        }
        else {
            success = false;
        }
        if (success == true) {
            // ��������Ƿ����Ű�Χ
            if (content.front() != '"' || content.back() != '"') {
                success = false;
            }
            else {
                // �Ƴ�����
                content = content.substr(1, content.size() - 2);
            }
        }

        return std::make_tuple(success, chatType, content);
	}
};