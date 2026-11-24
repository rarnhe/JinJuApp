#include "main.h"
#include <shellapi.h>
#include <shobjidl.h>
#include <UIAutomation.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <wrl/client.h>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>
#include <numeric>
#include <iterator>
#include <memory>
#include <climits>
#include <cwchar>
#include <utility>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
using Microsoft::WRL::ComPtr;
namespace jinju
{
constexpr wchar_t kClass[] = L"JinJuApp";
constexpr UINT_PTR QuoteTimer = 1, AnimationTimer = 2, GeometryTimer = 3;
DWORD OverlayStyles()
{
    return WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
}
constexpr char kDefault[] = R"JINJU(千里之行，始于足下。
九层之台，起于累土。
合抱之木，生于毫末。
水滴石穿，绳锯木断。
不积跬步，无以至千里。
不积小流，无以成江海。
锲而舍之，朽木不折。
锲而不舍，金石可镂。
业精于勤，荒于嬉。
行成于思，毁于随。
学而不思则罔，思而不学则殆。
温故而知新，可以为师矣。
三人行，必有我师焉。
择其善者而从之，其不善者而改之。
知之为知之，不知为不知，是知也。
学而时习之，不亦说乎。
人不知而不愠，不亦君子乎。
己所不欲，勿施于人。
己欲立而立人，己欲达而达人。
君子坦荡荡，小人长戚戚。
君子和而不同，小人同而不和。
君子喻于义，小人喻于利。
士不可以不弘毅，任重而道远。
岁寒，然后知松柏之后凋也。
生于忧患，死于安乐。
富贵不能淫，贫贱不能移，威武不能屈。
天将降大任于斯人也，必先苦其心志。
穷则独善其身，达则兼济天下。
老骥伏枥，志在千里。
烈士暮年，壮心不已。
天行健，君子以自强不息。
地势坤，君子以厚德载物。
穷则变，变则通，通则久。
流水不争先，争的是滔滔不绝。
知命不惧，日日自新。
功不唐捐，玉汝于成。
日拱一卒，功不唐捐。
一勤天下无难事。
志之所趋，无远弗届。
路漫漫其修远兮，吾将上下而求索。
长风破浪会有时，直挂云帆济沧海。
千淘万漉虽辛苦，吹尽狂沙始到金。
不经一番寒彻骨，怎得梅花扑鼻香。
纸上得来终觉浅，绝知此事要躬行。
横看成岭侧成峰，远近高低各不同。
不畏浮云遮望眼，自缘身在最高层。
山重水复疑无路，柳暗花明又一村。
沉舟侧畔千帆过，病树前头万木春。
问渠那得清如许，为有源头活水来。
等闲识得东风面，万紫千红总是春。
大漠孤烟直，长河落日圆。
会当凌绝顶，一览众山小。
欲穷千里目，更上一层楼。
莫愁前路无知己，天下谁人不识君。
海内存知己，天涯若比邻。
劝君莫惜金缕衣，劝君惜取少年时。
少年易老学难成，一寸光阴不可轻。
一寸山河一寸血，一抔热土一抔魂。
不谋万世者，不足谋一时。
天下大事，必作于细。
功崇惟志，业广惟勤。
志之所向，金石为开。
路虽远，行则将至。
事虽难，做则必成。
征途漫漫，惟有奋斗。
星光不问赶路人，历史属于奋斗者。
追光的人，终会光芒万丈。
追风赶月莫停留，平芜尽处是春山。
山再高，往上攀，总能登顶。
路再长，走下去，定能到达。
有风有雨是常态，风雨无阻是心态，风雨兼程是状态。
事不避难，义不逃责。
守正且创新，身正且行直，怀珠且抱玉。
乘风好去，长空万里，直下看山河。
逐梦不止，奋斗不息。
在该奋斗的岁月里，对得起每一寸光阴。
时间从不辜负对拼搏者的允诺，汗水从不吝啬对奋斗者的褒奖。
最好的状态，不是从不失误，而是从没放弃成长。
所谓的人生开挂，不过是厚积薄发。
你只管去做正确而重要的事，剩下的时间自会给你答案。
聚阳才能生焰，拢指才能聚拳。
船的力量在帆上，人的力量在心上。
拼手速的时代，不妨让子弹多飞一会儿。
衣白褂，破楼兰，赤子切记平安还。
心中有信仰，脚下有力量。
信仰如旗，引领方向；信念如磐，凝聚力量。
在平凡的岗位上燃烧，就是最耀眼的星辰。
伟大源于日复一日的坚守。
没有天生的英雄，只有挺身而出的凡人。
苔花如米小，也学牡丹开。
青春是用来奋斗的，不是用来挥霍的。
愿你成为自己的太阳，无需借谁的光。
关关难过关关过，夜夜难熬夜夜熬。
悄悄崩溃，默默自愈。
一个人使劲踮起脚尖靠近太阳的时候，全世界都挡不住她的阳光。
别贪心，我们不可能什么都有，别灰心，我们不可能什么都没有。
只要开始出发，就赢过了那些还停留在起点的人。
真正的优秀不是别人逼出来的，而是自己和自己死磕。
种一棵树最好是十年前，其次是现在。
人活着发自己的光就好，何必吹灭别人的光。
任何消耗你的人和事，多看一眼都是你的不对。
别在烂人烂事上纠缠，保护好你所有的正能量。
不要提前焦虑，也不要预支烦恼，生活就是见招拆招。
日落归山海，山海藏深意。
我们没有办法延续生命的长度，却可以增加生命的宽度和厚度。
把平凡的日子过出光，是生活最动人的本事。
允许自己做自己，也允许别人做别人。
请不要怀疑读书的价值，请不要误解善良的力量，请不要低估吃苦的回报。
生活不会辜负笨小孩，它只辜负装睡的人。
望远镜可以望见远方的目标，却不能代替你走半步路。
人生最好的贵人，就是努力向上的自己。
岁月漫长，心怀热爱，携手共赴星辰大海。
在暗处执着生长，终有一日馥郁传香。
总有人间一两风，填我十万八千梦。
晚风轻起时，愿你有好梦。
真正让你累的不是路远，而是鞋里的那粒沙。
愿你纵踩淤泥，也要心向光明。
身处低谷，怎么走都是向上。
你看，一个人之所以快乐，正是因为懂得向前看。
将不顺心的事甩在身后，才能看到未来的希望之光。
你的孤独虽败犹荣，你要悄悄拔尖，然后惊艳所有人。
你不能把这个世界，让给你所鄙视的人。
黑夜给了我黑色的眼睛，我却用它寻找光明。
面朝大海，春暖花开。
所有优秀背后，都是苦行僧般的自律。
生活永远不可能像你想象得那么好，但也不会像你想象得那么糟。
世界上只有一种真正的英雄主义，那就是认清生活的真相后还依然热爱生活。
幸福的家庭都是相似的，不幸的家庭各有各的不幸。
横眉冷对千夫指，俯首甘为孺子牛。
其实地上本没有路，走的人多了，也便成了路。
愿中国青年都摆脱冷气，只是向上走，不必听自暴自弃者流的话。
孤独是一个人的狂欢，狂欢是一群人的孤独。
真正的勇气不是没有恐惧，而是带着恐惧依然前行。
为你，千千万万遍。
那时候还年轻，不知道所有命运赠送的礼物，早已在暗中标好了价格。
三毛说，读书多了，容颜自然改变，许多时候自己可能以为看过的书籍都成了过眼云烟。
人最宝贵的是生命，生命每个人只有一次。
我们生而破碎，用活着来修修补补。
一切过往，皆为序章。
真正的勇敢，是明知风险仍选择担当。
敢于退让，是勇敢的另一种姿态。
读书是门槛最低的高贵，思考是成本最低的成长。
那些读过的文字终会沉淀成你的风骨。
坚持一次早起，多看一页书，多记一个单词。
短期看似微小，长期积累便是人生的分水岭。
读书是输入，思考是沉淀，行动是输出，坚持是复利。
学习没有捷径，但有方法。
没有方法的努力，只是看起来很努力。
不要假装很努力，结果不会陪你演戏。
每天进步一点点，复利效应会让你惊叹。
不怕慢，只怕站，不怕走得慢，就怕原地转。
精力是最珍贵的资源，对每个人皆一视同仁。
一个人的成长底色，由所读之书来铺陈。
那些读过的书，都会在未来铺就你脚下的路。
学过的每一点知识，都会在某一天以意想不到的方式回馈到你身上。
你所看到的惊艳，都曾被平庸历练。
比你优秀的人还比你努力，这不是内卷，而是提醒和砥砺。
如果一艘船不知道该驶向哪个港口，那么任何方向的风都不会是顺风。
于高山之巅，方见大河奔涌；于群峰之上，更觉长风浩荡。
众智谋事必明，众力举事必成。
向着阳光生长，黑夜就不再漫长。
所有的一鸣惊人，其实都是厚积薄发。
每一个不曾起舞的日子，都是对生命的辜负。
时间是最好的裁判，努力是最响亮的答案。
十年寒窗无人问，一举成名天下知。
把每一天活成种子，未来自有森林。
当下的耕耘，决定明天的收获。
星光不问赶路人，时光不负有心人。
今天多学一个知识点，明天就少说一句求人的话。
细节决定成败，态度决定一切，格局决定结局。
选择比努力更重要，但努力是选择的基础。
成功没有捷径，只有非走不可的远路。
有一种坚韧，叫靠自己。
名的背后是业，把业做到精湛，名自扬。
越努力，越幸运；越担当，越成长。
不是所有坚持都有结果，但所有有结果的坚持都需要方向。
方法不对，努力白费；方向不对，越努力越错。
先做正确的事，再正确地做事。
抬头看路，低头拉车，既要仰望星空，又要脚踏实地。
想都是问题，做才是答案，空想千遍，不如动手一遍。
敢于尝试，才能突破边界；敢于担当，才能赢得信任。
严于律己，宽以待人；做事有底线，做人有温度。
守住初心，方得始终；守住耐心，终见花开；守住信心，无畏前行。
心在一艺，其艺必工；心在一职，其职必举。
有多大本事，干多大事；有多大格局，成多大业。
把平凡的事做好，就是不平凡；把简单的事做精，就是不简单。
今天最好的表现，是明天最低的要求。
永远保持学习的心态，永远保持空杯的姿态，永远保持向上的状态。
路虽远，行则将至；事虽难，做则必成。
业精于勤而荒于嬉，行成于思而毁于随。
天行健，君子以自强不息；地势坤，君子以厚德载物。
道阻且长，行则将至；行而不辍，未来可期。
功成不必在我，功成必定有我。
国之大者，在于民生；民之根本，在于发展。
为官一任，造福一方。
既当改革的促进派，又当改革的实干家。
一分部署，九分落实。
干部时时上心，群众才能事事放心。
缺氧不缺精神、艰苦不怕吃苦、海拔高境界更高。
一个人如果不看书，那么他的价值观就会由他身边的人决定，因为他没有别的输入途径。
那些读过的书，都会在未来铺就你脚下的路；学过的每一点知识，都会在某一天回馈你。
先努力优秀，再大方拥有。眼界不够，看到的都是问题；格局不够，纠结的都是鸡毛蒜皮。
追求的后面没有句号，人生也永远没有太晚的开始，只要你听从内心的召唤。
沉淀，是一种人生修行。在沉淀的时光里，过滤掉浮躁与虚荣，不随波逐流，也不盲目跟风。
把拼搏交给当下，把收获交给时光。当你倾尽全力，必然会收获满意的成果。
少年就是少年，他看春风不喜，看夏蝉不烦，看秋风不悲，看冬雪不叹，只因他是少年。
正是青春信仰让我们的精神天空更为辽阔，正是青春追求让我们的时代画卷更为绚烂。
那些在无人问津的日子里的努力，都将成为日后惊艳众人的资本。
你今天的日积月累，终会变成别人的望尘莫及，你今天的全力以赴，终会换来明天的硕果累累。
)JINJU";
bool SameRect(const RECT &a, const RECT &b, int t)
{
    return std::abs(static_cast<long long>(a.left) - b.left) <= t &&
           std::abs(static_cast<long long>(a.top) - b.top) <= t &&
           std::abs(static_cast<long long>(a.right) - b.right) <= t &&
           std::abs(static_cast<long long>(a.bottom) - b.bottom) <= t;
}
Frame Marquee(int view, int text, int pad, double speed, double ms)
{
    int travel = std::max(0, text - std::max(0, view - 2 * pad));
    if (!travel || speed <= 0)
        return {pad, 1};
    double slide = travel * 1000.0 / speed, t = std::fmod(std::max(0.0, ms), 3100 + slide);
    if (t <= 2000)
        return {pad, 1};
    t -= 2000;
    if (t <= slide)
        return {pad - static_cast<int>(std::lround(t * speed / 1000)), 1};
    t -= slide;
    if (t <= 600)
        return {pad - travel, 1};
    t -= 600;
    if (t <= 250)
        return {pad - travel, 1 - t / 250};
    return {pad, std::clamp((t - 250) / 250, 0.0, 1.0)};
}
double Crossfade(double ms)
{
    return std::clamp(ms / 400.0, 0.0, 1.0);
}
std::optional<RECT> FreeStrip(RECT bar, std::vector<RECT> occupied, int margin, int minimum)
{
    if (occupied.empty() || bar.right <= bar.left || bar.bottom <= bar.top)
        return {};
    std::vector<std::pair<LONG, LONG>> spans;
    for (auto r : occupied)
    {
        RECT i{};
        if (IntersectRect(&i, &bar, &r))
            spans.emplace_back(i.left, i.right);
    }
    if (spans.empty())
        return {};
    std::sort(spans.begin(), spans.end());
    LONG cursor = bar.left;
    std::optional<RECT> best;
    auto gap = [&](LONG l, LONG r) {
        if (!best && r - l >= std::max(1, minimum) + 2 * margin)
            best = RECT{l + margin, bar.top, r - margin, bar.bottom};
    };
    for (auto [l, r] : spans)
    {
        gap(cursor, l);
        cursor = std::max(cursor, r);
    }
    gap(cursor, bar.right);
    return best;
}
std::vector<std::wstring> ParseLines(const std::string &bytes)
{
    std::vector<std::wstring> result;
    size_t start = bytes.starts_with("\xef\xbb\xbf") ? 3 : 0;
    while (start < bytes.size())
    {
        auto end = bytes.find('\n', start);
        if (end == std::string::npos)
            end = bytes.size();
        auto line = bytes.substr(start, end - start);
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.find_first_not_of(" \t") != std::string::npos)
        {
            if (line.size() > INT_MAX)
                return {};
            int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line.data(),
                                        static_cast<int>(line.size()), nullptr, 0);
            if (!n)
                return {};
            std::wstring s(n, L'\0');
            if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line.data(),
                                     static_cast<int>(line.size()), s.data(), n))
                return {};
            result.push_back(std::move(s));
        }
        if (end == bytes.size())
            break;
        start = end + 1;
    }
    return result;
}
static std::wstring ExePath()
{
    std::vector<wchar_t> b(32768);
    DWORD n = GetModuleFileNameW(nullptr, b.data(), static_cast<DWORD>(b.size()));
    return n && n < b.size() ? std::wstring(b.data(), n) : L"";
}
std::wstring GetProgramName()
{
    return std::filesystem::path(ExePath()).stem().wstring();
}
std::wstring QuotePath(const std::wstring &exe)
{
    if (exe.empty())
        return {};
    auto p = std::filesystem::path(exe);
    p.replace_extension(L".txt");
    return p.wstring();
}
std::vector<std::wstring> LoadQuotes(const std::wstring &path)
{
    if (path.empty())
        return {};
    HANDLE f =
        CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE)
    {
        DWORD n = 0;
        BOOL ok = WriteFile(f, kDefault, static_cast<DWORD>(sizeof(kDefault) - 1), &n, nullptr);
        CloseHandle(f);
        if (!ok || n != sizeof(kDefault) - 1)
            return {};
    }
    else if (GetLastError() != ERROR_FILE_EXISTS && GetLastError() != ERROR_ALREADY_EXISTS)
        return {};
    std::ifstream in(std::filesystem::path(path), std::ios::binary);
    if (!in)
        return {};
    std::string bytes((std::istreambuf_iterator<char>(in)), {});
    if (in.bad())
        return {};
    return ParseLines(bytes);
}
uint32_t WallpaperPixel(uint32_t rgb, unsigned coverage, double opacity)
{
    unsigned a = static_cast<unsigned>(std::lround(std::min(coverage, 255u) * std::clamp(opacity, 0.0, 1.0)));
    auto ch = [&](unsigned s) { return (((rgb >> s) & 255) * a + 127) / 255; };
    return (a << 24) | (ch(16) << 16) | (ch(8) << 8) | ch(0);
}
struct Dib
{
    HDC dc = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ old = nullptr;
    uint32_t *pixels = nullptr;
    int w, h;
    Dib(int width, int height) : w(width), h(height)
    {
        dc = CreateCompatibleDC(nullptr);
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void *raw = nullptr;
        if (dc)
            bmp = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &raw, nullptr, 0);
        if (bmp)
        {
            old = SelectObject(dc, bmp);
            if (old && old != HGDI_ERROR)
                pixels = static_cast<uint32_t *>(raw);
        }
    }
    ~Dib()
    {
        if (old && old != HGDI_ERROR)
            SelectObject(dc, old);
        if (bmp)
            DeleteObject(bmp);
        if (dc)
            DeleteDC(dc);
    }
    Dib(const Dib &) = delete;
    Dib &operator=(const Dib &) = delete;
    explicit operator bool() const
    {
        return pixels != nullptr;
    }
    void clear()
    {
        GdiFlush();
        std::fill_n(pixels, static_cast<size_t>(w) * h, 0u);
    }
};
static int Scale(int x, UINT dpi)
{
    return MulDiv(x, static_cast<int>(dpi), 96);
}
static bool Fullscreen(HWND strip, RECT bar)
{
    QUERY_USER_NOTIFICATION_STATE s{};
    if (SUCCEEDED(SHQueryUserNotificationState(&s)) && (s == QUNS_QUIET_TIME || s == QUNS_PRESENTATION_MODE))
        return true;
    HWND fg = GetForegroundWindow();
    if (!fg || fg == strip || fg == GetShellWindow() || fg == GetDesktopWindow() || IsIconic(fg))
        return false;
    wchar_t cls[128]{};
    GetClassNameW(fg, cls, 128);
    if (!wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") || !wcscmp(cls, L"Shell_TrayWnd"))
        return false;
    MONITORINFO mi{sizeof(mi)};
    HMONITOR m = MonitorFromRect(&bar, MONITOR_DEFAULTTONEAREST);
    if (MonitorFromWindow(fg, MONITOR_DEFAULTTONULL) != m || !GetMonitorInfoW(m, &mi))
        return false;
    RECT r{};
    if (FAILED(DwmGetWindowAttribute(fg, DWMWA_EXTENDED_FRAME_BOUNDS, &r, sizeof(r))) &&
        !GetWindowRect(fg, &r))
        return false;
    return SameRect(r, mi.rcMonitor, 2);
}
struct Anchors
{
    RECT bar;
    std::vector<RECT> items;
    bool start = false, task = false, tray = false;
};
static BOOL CALLBACK AnchorProc(HWND hwnd, LPARAM lp)
{
    auto &a = *reinterpret_cast<Anchors *>(lp);
    if (!IsWindowVisible(hwnd))
        return TRUE;
    wchar_t cls[128]{};
    GetClassNameW(hwnd, cls, 128);
    bool start = !wcscmp(cls, L"Start") || !wcscmp(cls, L"StartButton"),
         task = !wcscmp(cls, L"MSTaskListWClass"), tray = !wcscmp(cls, L"TrayNotifyWnd");
    if (!(start || task || tray))
        return TRUE;
    RECT r{}, i{};
    if (!GetWindowRect(hwnd, &r) || !IntersectRect(&i, &a.bar, &r))
        return TRUE;
    // Task-list HWND can cover the entire available region, not just buttons.
    if (!task || i.right - i.left < (a.bar.right - a.bar.left) / 2)
        a.items.push_back(i);
    a.start |= start;
    a.task |= task;
    a.tray |= tray;
    return TRUE;
}
struct UiaScan
{
    std::vector<RECT> items;
    int buttons = 0, visited = 0;
    bool complete = true;
};
static void ScanUia(IUIAutomationTreeWalker *walker, IUIAutomationElement *node, RECT bar, UiaScan &out,
                    int depth)
{
    if (depth > 24 || ++out.visited > 1000)
    {
        out.complete = false;
        return;
    }
    CONTROLTYPEID type = 0;
    BOOL off = TRUE;
    RECT r{}, i{};
    if (FAILED(node->get_CurrentControlType(&type)) || FAILED(node->get_CurrentIsOffscreen(&off)) ||
        FAILED(node->get_CurrentBoundingRectangle(&r)))
    {
        out.complete = false;
        return;
    }
    bool hit = !off && IntersectRect(&i, &bar, &r) && r.top + (r.bottom - r.top) / 2 >= bar.top &&
               r.top + (r.bottom - r.top) / 2 < bar.bottom;
    bool control = type == UIA_ButtonControlTypeId || type == UIA_SplitButtonControlTypeId ||
                   type == UIA_MenuItemControlTypeId || type == UIA_ListItemControlTypeId ||
                   type == UIA_TabItemControlTypeId || type == UIA_EditControlTypeId ||
                   type == UIA_CheckBoxControlTypeId || type == UIA_HyperlinkControlTypeId ||
                   type == UIA_TextControlTypeId || type == UIA_ComboBoxControlTypeId;
    if (depth && hit && control)
    {
        out.items.push_back(i);
        if (type == UIA_ButtonControlTypeId || type == UIA_SplitButtonControlTypeId)
            ++out.buttons;
    }
    ComPtr<IUIAutomationElement> child;
    if (FAILED(walker->GetFirstChildElement(node, &child)))
    {
        out.complete = false;
        return;
    }
    while (child && out.complete)
    {
        ScanUia(walker, child.Get(), bar, out, depth + 1);
        ComPtr<IUIAutomationElement> next;
        if (FAILED(walker->GetNextSiblingElement(child.Get(), &next)))
        {
            out.complete = false;
            return;
        }
        child = std::move(next);
    }
}
static std::optional<RECT> TaskbarStrip(IUIAutomation *uia, RECT &bar, UINT &dpi)
{
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindowVisible(tray) || !GetWindowRect(tray, &bar))
        return {};
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(MonitorFromWindow(tray, MONITOR_DEFAULTTONEAREST), &mi))
        return {};
    if (bar.bottom != mi.rcMonitor.bottom || bar.right - bar.left <= bar.bottom - bar.top ||
        bar.bottom - bar.top < 12)
        return {};
    APPBARDATA abd{sizeof(abd)};
    if (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE)
        return {};
    dpi = GetDpiForWindow(tray);
    if (!dpi)
        dpi = 96;
    Anchors a{bar};
    EnumChildWindows(tray, AnchorProc, reinterpret_cast<LPARAM>(&a));
    UiaScan s;
    if (uia)
    {
        ComPtr<IUIAutomationElement> root;
        ComPtr<IUIAutomationTreeWalker> walker;
        if (SUCCEEDED(uia->ElementFromHandle(tray, &root)) && root &&
            SUCCEEDED(uia->get_RawViewWalker(&walker)) && walker)
            ScanUia(walker.Get(), root.Get(), bar, s, 0);
        else
            s.complete = false;
    }
    else
        s.complete = false;
    // Do not guess free space if the accessibility provider failed.
    if (!s.complete || s.buttons < 2 || !a.tray)
        return {};
    a.items.insert(a.items.end(), s.items.begin(), s.items.end());
    return FreeStrip(bar, std::move(a.items), 7, 0);
}
// Resolve the actual wallpaper for the monitor; never capture the composed
// screen, since that would copy the taskbar and our own text back into itself.
static bool PaintWallpaper(Dib &dst, RECT strip, IDesktopWallpaper *desktop)
{
    HMONITOR monitor = MonitorFromRect(&strip, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfoW(monitor, &mi))
        return false;

    std::wstring file;
    COLORREF background = GetSysColor(COLOR_BACKGROUND);
    DESKTOP_WALLPAPER_POSITION position = DWPOS_FILL;
    bool resolved = false;
    if (desktop)
    {
        desktop->GetBackgroundColor(&background);
        desktop->GetPosition(&position);
        UINT count = 0;
        if (SUCCEEDED(desktop->GetMonitorDevicePathCount(&count)))
        {
            for (UINT i = 0; i < count; ++i)
            {
                LPWSTR id = nullptr;
                RECT monitorRect{};
                if (SUCCEEDED(desktop->GetMonitorDevicePathAt(i, &id)) && id &&
                    SUCCEEDED(desktop->GetMonitorRECT(id, &monitorRect)) &&
                    SameRect(monitorRect, mi.rcMonitor))
                {
                    LPWSTR path = nullptr;
                    if (SUCCEEDED(desktop->GetWallpaper(id, &path)))
                    {
                        resolved = true;
                        if (path)
                            file = path;
                    }
                    CoTaskMemFree(path);
                    CoTaskMemFree(id);
                    break;
                }
                CoTaskMemFree(id);
            }
        }
    }

    // SPI is a second system source, useful on the primary monitor if
    // IDesktopWallpaper did not return a usable image path.
    std::wstring fallback;
    if (mi.dwFlags & MONITORINFOF_PRIMARY)
    {
        std::vector<wchar_t> buffer(32768);
        if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, static_cast<UINT>(buffer.size()), buffer.data(), 0))
        {
            fallback = buffer.data();
            if (!resolved && fallback.empty())
                resolved = true;
        }
    }
    if (file.empty() && !fallback.empty())
    {
        file = fallback;
        resolved = true;
    }
    if (!resolved)
        return false;

    RECT area = mi.rcMonitor;
    if (position == DWPOS_SPAN)
    {
        area.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
        area.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
        area.right = area.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
        area.bottom = area.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
    }

    bool ok = RasterWallpaper(dst.pixels, dst.w, dst.h, strip, area, file, position, background);
    if (!ok && !fallback.empty() && fallback != file)
    {
        file = fallback;
        ok = RasterWallpaper(dst.pixels, dst.w, dst.h, strip, area, file, position, background);
    }
    if (!ok)
    {
        OutputDebugStringW(L"JinJuApp: wallpaper decoding failed; hiding strip.\n");
        return false;
    }
    // Useful in DebugView / Visual Studio without writing an extra user file.
    uint32_t minRGB = 0xffffff, maxRGB = 0;
    size_t colored = 0;
    const size_t n = static_cast<size_t>(dst.w) * dst.h;
    for (size_t i = 0; i < n; ++i)
    {
        const auto rgb = dst.pixels[i] & 0xffffff;
        minRGB = std::min(minRGB, rgb);
        maxRGB = std::max(maxRGB, rgb);
        colored += rgb != 0;
    }
    wchar_t log[256]{};
    swprintf_s(log,
               L"JinJuApp wallpaper: position=%d rect=(%ld,%ld,%ld,%ld) nonblack=%zu/%zu range=%06X..%06X\n",
               static_cast<int>(position), strip.left, strip.top, strip.right, strip.bottom, colored, n,
               static_cast<unsigned>(minRGB), static_cast<unsigned>(maxRGB));
    OutputDebugStringW(log);
    return true;
}

// All source and destination coordinates are physical screen pixels.
// GDI+ only decodes the source; the actual wallpaper sampler is explicit.
bool RasterWallpaper(uint32_t *pixels, int width, int height, RECT strip, RECT area, const std::wstring &file,
                     int position, COLORREF background)
{
    if (!pixels || width <= 0 || height <= 0 || area.right <= area.left || area.bottom <= area.top)
        return false;
    const uint32_t bg = 0xff000000u | (static_cast<uint32_t>(GetRValue(background)) << 16) |
                        (static_cast<uint32_t>(GetGValue(background)) << 8) | GetBValue(background);
    std::fill_n(pixels, static_cast<size_t>(width) * height, bg);
    if (file.empty())
        return true;

    Gdiplus::Bitmap source(file.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok || !source.GetWidth() || !source.GetHeight() ||
        source.GetWidth() > INT_MAX || source.GetHeight() > INT_MAX)
        return false;
    const int iw = static_cast<int>(source.GetWidth());
    const int ih = static_cast<int>(source.GetHeight());
    Gdiplus::Rect bounds(0, 0, iw, ih);
    // Request an explicit 32bpp ARGB conversion even for JPEG/24-bit BMP.
    Gdiplus::BitmapData data{};
    if (source.LockBits(&bounds, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) != Gdiplus::Ok)
        return false;

    const double aw = area.right - area.left;
    const double ah = area.bottom - area.top;
    double dw = iw, dh = ih;
    if (position == DWPOS_STRETCH)
    {
        dw = aw;
        dh = ah;
    }
    else if (position != DWPOS_CENTER && position != DWPOS_TILE)
    {
        const double scale = position == DWPOS_FIT ? std::min(aw / iw, ah / ih) : std::max(aw / iw, ah / ih);
        dw *= scale;
        dh *= scale;
    }
    const double originX = area.left + (aw - dw) / 2;
    const double originY = area.top + (ah - dh) / 2;
    auto wrap = [](int v, int n) { return ((v % n) + n) % n; };
    auto sample = [&](int x, int y) {
        x = std::clamp(x, 0, iw - 1);
        y = std::clamp(y, 0, ih - 1);
        const auto *row = reinterpret_cast<const uint32_t *>(static_cast<const BYTE *>(data.Scan0) +
                                                             static_cast<ptrdiff_t>(y) * data.Stride);
        return row[x];
    };
    auto opaque = [&](uint32_t p) {
        const unsigned a = p >> 24;
        auto channel = [&](unsigned shift) {
            return (((p >> shift) & 255) * a + ((bg >> shift) & 255) * (255 - a) + 127) / 255;
        };
        return 0xff000000u | (channel(16) << 16) | (channel(8) << 8) | channel(0);
    };
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const double sx = strip.left + x + 0.5;
            const double sy = strip.top + y + 0.5;
            auto &pixel = pixels[static_cast<size_t>(y) * width + x];
            if (position == DWPOS_TILE)
            {
                pixel =
                    opaque(sample(wrap(strip.left + x - area.left, iw), wrap(strip.top + y - area.top, ih)));
                continue;
            }
            if (sx < originX || sx >= originX + dw || sy < originY || sy >= originY + dh)
                continue;
            const double u = (sx - originX) * iw / dw - 0.5;
            const double v = (sy - originY) * ih / dh - 0.5;
            const int ix = static_cast<int>(std::floor(u));
            const int iy = static_cast<int>(std::floor(v));
            const double fx = u - ix, fy = v - iy;
            const uint32_t p00 = opaque(sample(ix, iy));
            const uint32_t p10 = opaque(sample(ix + 1, iy));
            const uint32_t p01 = opaque(sample(ix, iy + 1));
            const uint32_t p11 = opaque(sample(ix + 1, iy + 1));
            auto channel = [&](unsigned shift) {
                const double a = ((p00 >> shift) & 255) * (1 - fx) + ((p10 >> shift) & 255) * fx;
                const double b = ((p01 >> shift) & 255) * (1 - fx) + ((p11 >> shift) & 255) * fx;
                return static_cast<uint32_t>(std::lround(a * (1 - fy) + b * fy));
            };
            pixel = 0xff000000u | (channel(16) << 16) | (channel(8) << 8) | channel(0);
        }
    }
    return source.UnlockBits(&data) == Gdiplus::Ok;
}
static HFONT MakeFont(UINT dpi)
{
    NONCLIENTMETRICSW metrics{sizeof(metrics)};
    LOGFONTW lf{};
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        lf = metrics.lfMessageFont;
    lf.lfHeight = -MulDiv(13, static_cast<int>(dpi), 72);
    lf.lfWeight = FW_LIGHT;
    lf.lfQuality = ANTIALIASED_QUALITY;
    return CreateFontIndirectW(&lf);
}
struct GlyphMask
{
    int w = 0, h = 0;
    std::vector<uint8_t> coverage;
};
static GlyphMask RasterText(const std::wstring &text, HFONT font, int width, int height)
{
    GlyphMask out;
    if (width <= 0 || height <= 0 || text.empty() || text.size() > INT_MAX)
        return out;
    // Limit the cached bitmap to a reasonable size; extremely long custom
    // lines use the original clipped GDI rendering fallback.
    if (static_cast<uint64_t>(width) * height > 16 * 1024 * 1024)
        return out;
    Dib dib(width, height);
    if (!dib)
        return out;
    dib.clear();
    auto old = SelectObject(dib.dc, font);
    RECT bounds{0, 0, width, height};
    FillRect(dib.dc, &bounds, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    SetBkMode(dib.dc, OPAQUE);
    SetBkColor(dib.dc, RGB(0, 0, 0));
    SetTextColor(dib.dc, RGB(255, 255, 255));
    DrawTextW(dib.dc, text.c_str(), static_cast<int>(text.size()), &bounds,
              DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP);
    SelectObject(dib.dc, old);
    GdiFlush();
    out.w = width;
    out.h = height;
    out.coverage.resize(static_cast<size_t>(width) * height);
    for (size_t i = 0; i < out.coverage.size(); ++i)
        out.coverage[i] = static_cast<uint8_t>((dib.pixels[i] >> 16) & 255);
    return out;
}
bool TestGlyphComposition()
{
    HFONT font = MakeFont(96);
    if (!font)
        return false;
    auto mask = RasterText(L"Windows test", font, 180, 48);
    DeleteObject(font);
    bool ink = false, transparent = false;
    for (auto c : mask.coverage)
    {
        auto p = WallpaperPixel(0x42a9ef, c, 1);
        if (!c)
            transparent |= p == 0;
        if (c >= 240 && (p & 0xff) >= 224)
            ink = true;
    }
    return ink && transparent;
}
struct GeometrySnapshot
{
    RECT bar{};
    UINT dpi = 96;
    std::optional<RECT> target;
};
struct GeometryWorker
{
    std::mutex mutex;
    GeometrySnapshot snapshot;
    std::jthread thread;
    void Start()
    {
        thread = std::jthread([this](std::stop_token stop) {
            HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            if (FAILED(hr))
                return;
            {
                ComPtr<IUIAutomation> uia;
                CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(uia.GetAddressOf()));
                while (!stop.stop_requested())
                {
                    GeometrySnapshot next;
                    next.target = TaskbarStrip(uia.Get(), next.bar, next.dpi);
                    {
                        std::lock_guard lock(mutex);
                        snapshot = next;
                    }
                    for (int i = 0; i < 10 && !stop.stop_requested(); ++i)
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            CoUninitialize();
        });
    }
    GeometrySnapshot Get()
    {
        std::lock_guard lock(mutex);
        return snapshot;
    }
};
struct App
{
    HWND hwnd = nullptr;
    GeometryWorker geometry;
    ComPtr<IDesktopWallpaper> desktop;
    std::vector<std::wstring> quotes;
    std::vector<size_t> bag;
    size_t cursor = 0;
    std::mt19937 rng{std::random_device{}()};
    std::wstring current, previous;
    RECT strip{};
    UINT dpi = 96;
    bool active = false;
    ULONGLONG start = 0, changed = 0;
    Frame oldFrame{0, 1};
    int textWidth = 0, lastX = INT_MIN, lastA = -1;
    std::unique_ptr<Dib> mask, frame, wall;
    HFONT font = nullptr;
    GlyphMask currentMask, previousMask;
    ULONGLONG lastTick = 0;
    double animationMs = 0;
    bool wallpaperDirty = false;
    unsigned missingGeometry = 0;
    ULONGLONG lastWallpaperPoll = 0;
    ~App()
    {
        if (font)
            DeleteObject(font);
    }
    std::wstring Next()
    {
        if (cursor == bag.size())
        {
            bag.resize(quotes.size());
            std::iota(bag.begin(), bag.end(), 0);
            std::shuffle(bag.begin(), bag.end(), rng);
            cursor = 0;
            if (bag.size() > 1 && quotes[bag[0]] == current)
                std::swap(bag[0], bag[1]);
        }
        return quotes[bag[cursor++]];
    }
    int Pad() const
    {
        return 2;
    }
    int Measure(const std::wstring &s)
    {
        if (!mask || !font || s.size() > INT_MAX)
            return 0;
        auto old = SelectObject(mask->dc, font);
        RECT bounds{0, 0, 0, mask->h};
        DrawTextW(mask->dc, s.c_str(), static_cast<int>(s.size()), &bounds,
                  DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);
        SelectObject(mask->dc, old);
        return std::max(0L, bounds.right);
    }
    Frame At(ULONGLONG /*now*/) const
    {
        return Marquee(strip.right - strip.left, textWidth, Pad(), 45.0 * dpi / 96.0, animationMs);
    }
    bool Rebuild()
    {
        int w = strip.right - strip.left, h = strip.bottom - strip.top;
        if (w <= 0 || h <= 0)
            return false;
        mask = std::make_unique<Dib>(w, h);
        frame = std::make_unique<Dib>(w, h);
        wall = std::make_unique<Dib>(w, h);
        if (!*mask || !*frame || !*wall)
            return false;
        if (font)
            DeleteObject(font);
        font = MakeFont(dpi);
        if (!font)
            return false;
        textWidth = Measure(current);
        currentMask = RasterText(current, font, textWidth + Scale(4, dpi), h);
        previousMask = {};
        previous.clear();
        start = lastTick = GetTickCount64();
        animationMs = 0;
        lastX = INT_MIN;
        lastA = -1;
        return PaintWallpaper(*wall, strip, desktop.Get());
    }
    void Layer(const std::wstring &s, const GlyphMask &cached, Frame f, double weight)
    {
        if (s.empty() || weight <= 0 || f.alpha <= 0 || s.size() > INT_MAX)
            return;
        const unsigned opacity = static_cast<unsigned>(std::lround(255 * f.alpha * weight));
        const bool useCache = !cached.coverage.empty();
        if (!useCache)
        {
            mask->clear();
            auto old = SelectObject(mask->dc, font);
            RECT background{0, 0, mask->w, mask->h};
            FillRect(mask->dc, &background, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            SetBkMode(mask->dc, OPAQUE);
            SetBkColor(mask->dc, RGB(0, 0, 0));
            SetTextColor(mask->dc, RGB(255, 255, 255));
            RECT bounds{f.x, 0, f.x + mask->w, mask->h};
            DrawTextW(mask->dc, s.c_str(), static_cast<int>(s.size()), &bounds,
                      DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_NOCLIP);
            SelectObject(mask->dc, old);
            GdiFlush();
        }
        const int left = useCache ? std::max(Pad(), f.x) : Pad();
        const int right = useCache ? std::min(mask->w - Pad(), f.x + cached.w) : mask->w - Pad();
        for (int y = 0; y < mask->h; ++y)
        {
            for (int x = left; x < right; ++x)
            {
                const size_t i = static_cast<size_t>(y) * mask->w + x;
                const unsigned c = useCache ? cached.coverage[static_cast<size_t>(y) * cached.w + x - f.x]
                                            : ((mask->pixels[i] >> 16) & 255);
                if (!c)
                    continue;
                const unsigned a = (c * opacity + 127) / 255;
                const unsigned total = std::min(255u, (frame->pixels[i] >> 24) + a);
                frame->pixels[i] = WallpaperPixel(wall->pixels[i], total, 1);
            }
        }
    }
    void Hide()
    {
        if (!active && !IsWindowVisible(hwnd))
            return;
        active = false;
        KillTimer(hwnd, AnimationTimer);
        ShowWindow(hwnd, SW_HIDE);
    }
    void Render(bool force = false)
    {
        if (!active || !mask || !frame || !wall)
            return;
        auto now = GetTickCount64();
        animationMs += static_cast<double>(std::min<ULONGLONG>(now - lastTick, 250));
        lastTick = now;
        auto f = At(now);
        double blend = previous.empty() ? 1 : Crossfade(static_cast<double>(now - changed));
        int aq = static_cast<int>(std::lround(f.alpha * 255));
        if (!force && previous.empty() && f.x == lastX && aq == lastA)
            return;
        frame->clear();
        Layer(previous, previousMask, oldFrame, 1 - blend);
        Layer(current, currentMask, f, blend);
        POINT dest{strip.left, strip.top}, src{};
        SIZE size{frame->w, frame->h};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        if (!UpdateLayeredWindow(hwnd, nullptr, &dest, &size, frame->dc, &src, 0, &bf, ULW_ALPHA))
        {
            Hide();
            return;
        }
        lastX = f.x;
        lastA = aq;
        if (blend >= 1)
        {
            previous.clear();
            previousMask = {};
        }
        if (previous.empty() && textWidth <= frame->w - 2 * Pad())
            KillTimer(hwnd, AnimationTimer);
    }
    void Animate()
    {
        if (active && (!previous.empty() || (frame && textWidth > frame->w - 2 * Pad())))
            SetTimer(hwnd, AnimationTimer, 15, nullptr);
    }
    void Refresh(bool wallpaper = false)
    {
        auto snapshot = geometry.Get();
        RECT bar = snapshot.bar;
        UINT d = snapshot.dpi;
        auto target = snapshot.target;
        HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
        RECT actualBar{};
        if (!taskbar || !GetWindowRect(taskbar, &actualBar) || Fullscreen(hwnd, actualBar))
        {
            Hide();
            return;
        }
        if (!target)
        {
            // A transient accessibility failure while clicking the taskbar must
            // not make the overlay flash. Two seconds without data hides it.
            if (++missingGeometry >= 8)
                Hide();
            return;
        }
        missingGeometry = 0;
        bool rebuild = !active || !SameRect(strip, *target) || dpi != d;
        bool refreshWallpaper = wallpaper || wallpaperDirty;
        wallpaperDirty = false;
        if (rebuild)
        {
            strip = *target;
            dpi = d;
            if (!Rebuild())
            {
                Hide();
                return;
            }
        }
        active = true;
        if (rebuild)
        {
            Render(true);
            if (!active)
                return;
            if (!IsWindowVisible(hwnd))
                ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            Animate();
        }
        else if (refreshWallpaper && wall)
        {
            if (!PaintWallpaper(*wall, strip, desktop.Get()))
            {
                Hide();
                return;
            }
            Render(true);
        }
        // Only repair Z order when the taskbar actually moved above us.
        // A periodic unconditional HWND_TOPMOST call can cause visible flicker.
        for (HWND above = GetWindow(hwnd, GW_HWNDPREV); above; above = GetWindow(above, GW_HWNDPREV))
        {
            if (above == taskbar)
            {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                break;
            }
        }
    }
    void Change()
    {
        auto now = GetTickCount64();
        oldFrame = At(now);
        previous = current;
        previousMask = std::move(currentMask);
        current = Next();
        start = changed = lastTick = now;
        animationMs = 0;
        textWidth = Measure(current);
        if (mask)
            currentMask = RasterText(current, font, textWidth + Scale(4, dpi), mask->h);
        lastX = INT_MIN;
        if (active)
        {
            if (!IsWindowVisible(hwnd))
                ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            Render(true);
            Animate();
        }
        else
            previous.clear();
    }
};
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_NCCREATE)
    {
        auto cs = reinterpret_cast<CREATESTRUCTW *>(lp);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }
    auto a = reinterpret_cast<App *>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (a && a->hwnd)
        switch (msg)
        {
        case WM_TIMER:
            if (wp == QuoteTimer)
                a->Change();
            else if (wp == AnimationTimer)
                a->Render();
            else if (wp == GeometryTimer)
                a->Refresh();
            return 0;
        case WM_SETTINGCHANGE:
            if (wp == SPI_SETDESKWALLPAPER || wp == 0)
                a->wallpaperDirty = true;
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            a->wallpaperDirty = true;
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT:
            ValidateRect(hwnd, nullptr);
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_POINTERACTIVATE:
            return PA_NOACTIVATE;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
} // namespace jinju
#ifndef JINJU_TEST
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    using namespace jinju;
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HANDLE mutex = CreateMutexW(nullptr, FALSE, L"Local\\JinJuApp-SingleInstance");
    if (!mutex)
        return 1;
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(mutex);
        return 0;
    }
    auto quotes = LoadQuotes(QuotePath(ExePath()));
    if (quotes.empty())
    {
        CloseHandle(mutex);
        return 0;
    }
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(co))
    {
        CloseHandle(mutex);
        return 1;
    }
    Gdiplus::GdiplusStartupInput gi;
    ULONG_PTR token = 0;
    if (Gdiplus::GdiplusStartup(&token, &gi, nullptr) != Gdiplus::Ok)
    {
        CoUninitialize();
        CloseHandle(mutex);
        return 1;
    }
    int result = 0;
    {
        App app;
        app.quotes = std::move(quotes);
        app.current = app.Next();
        app.start = GetTickCount64();
        app.geometry.Start();
        CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(app.desktop.GetAddressOf()));
        WNDCLASSEXW wc{sizeof(wc)};
        wc.hInstance = instance;
        wc.lpfnWndProc = WndProc;
        wc.lpszClassName = kClass;
        if (!RegisterClassExW(&wc))
            result = 1;
        else
        {
            app.hwnd = CreateWindowExW(OverlayStyles(), kClass, L"JinJuApp", WS_POPUP, 0, 0, 0, 0, nullptr,
                                       nullptr, instance, &app);
            if (!app.hwnd)
                result = 1;
            else
            {
                SetTimer(app.hwnd, QuoteTimer, 180000, nullptr);
                SetTimer(app.hwnd, GeometryTimer, 250, nullptr);
                app.Refresh();
                MSG msg{};
                BOOL status;
                while ((status = GetMessageW(&msg, nullptr, 0, 0)) > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                if (status == -1)
                    result = 1;
                if (IsWindow(app.hwnd))
                    DestroyWindow(app.hwnd);
            }
        }
    }
    Gdiplus::GdiplusShutdown(token);
    CoUninitialize();
    CloseHandle(mutex);
    return result;
}
#endif
