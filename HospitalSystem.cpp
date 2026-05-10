#define _CRT_SECURE_NO_WARNINGS
#include "HospitalSystem.hpp"
#include <fstream>
#include <ctime>

//  InputField implementation

void InputField::setup(sf::Font& f, float x, float y, float w, float h, int fs) {
    box.setPosition(x, y);
    box.setSize({ w, h });
    box.setOutlineThickness(2.f);
    txt.setFont(f);
    txt.setCharacterSize(fs);
    txt.setFillColor(sf::Color::White);
    txt.setPosition(x + 10.f, y + (h - fs) * 0.5f - 2.f);
}
void InputField::draw(sf::RenderWindow& w) {
    box.setFillColor(active ? sf::Color(38, 52, 85) : sf::Color(20, 30, 52));
    box.setOutlineColor(active ? sf::Color(0, 185, 230) : sf::Color(50, 72, 112));
    w.draw(box);
    char disp[512];
    if (isPwd) {
        for (int i = 0; i < len; i++) {
            disp[i] = '*';
        }
        disp[len] = '\0';
    }
    else {
        for (int i = 0; i <= len; i++) {
            disp[i] = buf[i];
        }
    }
    txt.setString(disp);
    w.draw(txt);
}
bool InputField::hit(sf::Vector2f p) const {
    return box.getGlobalBounds().contains(p);
}

//  Constructor / Destructor

HospitalManagementSystem::HospitalManagementSystem(): scr(SCR_ROLE), role(ROLE_NONE),
logID(0), failCount(0), locked(false),scrollY(0), activeField(nullptr), msgOn(false), msgErr(true),bkStep(0),
bkDocID(0), bkDateFails(0), prStep(0), prApptID(0),histSearched(false), histPatID(0), selApptID(0),
selBillID(0), selDocID(0), selPatID(0),secCount(0), reportReady(false), rptTotal(0), rptPend(0),
rptComp(0), rptNos(0), rptCanc(0), rptRev(0.f)
{
    *(msgBuf+0) = '\0';
   *(bkSpec+0) = *(bkDate + 0) = *(bkSlot + 0) = '\0';
}
HospitalManagementSystem::~HospitalManagementSystem() {}

//  run()

void HospitalManagementSystem::run() {
    win.create(sf::VideoMode(W, H),
        "MediCore Hospital Management System",
        sf::Style::Close | sf::Style::Titlebar);
    win.setFramerateLimit(60);
    loadFont();
    loadData();
    // one-time field position setup for login
    fldID.setup(font, 440, 316, 400, 44);
    fldPwd.setup(font, 440, 392, 400, 44);
    fldPwd.isPwd = true;
    while (win.isOpen()) {
        sf::Vector2f mouse = (sf::Vector2f)sf::Mouse::getPosition(win);
        sf::Event ev;
        while (win.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                win.close();
            }

            if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {
                onClick({ (float)ev.mouseButton.x, (float)ev.mouseButton.y });
            }

            if (ev.type == sf::Event::TextEntered && activeField && ev.text.unicode < 128 &&
                ev.text.unicode != 8 && ev.text.unicode != 13 && ev.text.unicode >= 32) {
                activeField->push((char)ev.text.unicode);
            }
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::BackSpace && activeField) {
                    activeField->pop();
                }
                if (ev.key.code == sf::Keyboard::Escape) {
                    back();
                }
            }
            if (ev.type == sf::Event::MouseWheelScrolled) {
                scrollY -= (int)(ev.mouseWheelScroll.delta * 30.f);
                if (scrollY < 0) {
                    scrollY = 0;
                }
            }
        }

        win.clear(sf::Color(10, 14, 28));
        drawBg();

        switch (scr) {
        case SCR_ROLE:
            drawRole(mouse);     
            break;
        case SCR_LOGIN:   
            drawLogin(mouse); 
            break;
        case SCR_PAT_MENU:    
            drawPatMenu(mouse);
            break;
        case SCR_PAT_BOOK:      
            drawPatBook(mouse);
            break;
        case SCR_PAT_CANCEL:
            drawPatCancel(mouse);
            break;
        case SCR_PAT_APPTS:
            drawPatAppts(mouse);
            break;
        case SCR_PAT_RECORDS:
            drawPatRecords(mouse);
            break;
        case SCR_PAT_BILLS:
            drawPatBills(mouse);
            break;
        case SCR_PAT_PAY:
            drawPatPay(mouse);
            break;
        case SCR_PAT_TOPUP:
            drawPatTopup(mouse);
            break;
        case SCR_DOC_MENU:
            drawDocMenu(mouse);
            break;
        case SCR_DOC_TODAY:
            drawDocToday(mouse);
            break;
        case SCR_DOC_COMPLETE:
            drawDocComplete(mouse);
            break;
        case SCR_DOC_NOSHOW:
            drawDocNoshow(mouse);
            break;
        case SCR_DOC_PRESCRIBE:
            drawDocPrescribe(mouse);
            break;
        case SCR_DOC_HISTORY:
            drawDocHistory(mouse);
            break;
        case SCR_ADM_MENU:
            drawAdmMenu(mouse); 
            break;
        case SCR_ADM_ADD_DOC: 
            drawAdmAddDoc(mouse); 
            break;
        case SCR_ADM_REM_DOC:
            drawAdmRemDoc(mouse); 
            break;
        case SCR_ADM_PATIENTS:
            drawAdmPatients(mouse);
            break;
        case SCR_ADM_DOCTORS:
            drawAdmDoctors(mouse);
            break;
        case SCR_ADM_APPTS:
            drawAdmAppts(mouse); 
            break;
        case SCR_ADM_UNPAID: 
            drawAdmUnpaid(mouse);
            break;
        case SCR_ADM_DISCHARGE:
            drawAdmDischarge(mouse);
            break;
        case SCR_ADM_SECLOG:
            drawAdmSeclog(mouse); 
            break;
        case SCR_ADM_REPORT:
            drawAdmReport(mouse);
            break;
        }
        drawMsg();
        drawFtr();
        win.display();
    }
}


//  Font / Data loading


void HospitalManagementSystem::loadFont() {
    const char* paths[] = { "arial.ttf","fonts/arial.ttf","C:/Windows/Fonts/arial.ttf", "C:/Windows/Fonts/calibri.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf","/usr/share/fonts/TTF/DejaVuSans.ttf","/Library/Fonts/Arial.ttf",
        nullptr};
    for (int i = 0; paths[i]; i++)
        if (font.loadFromFile(paths[i])) {
            return;
        }
}

void HospitalManagementSystem::loadData() {
    try { f.filePatients(storPat); }
    catch (...) {}
    try { f.fileDoctor(storDoc); }
    catch (...) {}
    try { f.fileAppointments(storApp); }
    catch (...) {}
    try { f.fileAdmins(storAdm); }
    catch (...) {}
    try { f.fileBills(storBill); }
    catch (...) {}
    try { f.filePrescriptions(storPres); }
    catch (...) {}
}

void HospitalManagementSystem::loadSecLog() {
    secCount = 0;
    std::ifstream f("security_log.txt");
    if (!f.is_open()) {
        return;
    }
    char line[260];
    while (f.getline(line, 260) && secCount < 80) {
        int i = 0;
        while (line[i] && i < 259) { 
            secLines[secCount][i] = line[i];
            i++; 
        }
        secLines[secCount][i] = '\0';
        secCount++;
    }
    f.close();
}

void HospitalManagementSystem::computeReport() {
    if (reportReady) {
        return;
    }
    char today[16];
    getTodayStr(today);
    rptTotal = rptPend = rptComp = rptNos = rptCanc = 0;
    rptRev = 0.f;
    for (int i = 0; i < storApp.getSize(); i++) {
        Appointment& a = storApp[i];
        if (!isEqual(a.getDate(), today)) {
            continue;
        }
        rptTotal++;
        if (isEqual(a.getStatus(), "pending")) {
            rptPend++;
        }
        else if (isEqual(a.getStatus(), "completed")) {
            rptComp++;
        }
        else if (isEqual(a.getStatus(), "no-show")) {
            rptNos++;
        }
        else if (isEqual(a.getStatus(), "cancelled")) {
            rptCanc++;
        }
    }
    for (int i = 0; i < storBill.getSize(); i++) {
        Bill& b = storBill[i];
        if (isEqual(b.getStatus(), "paid") && isEqual(b.getDate(), today)) {
            rptRev += b.getAmount();
        }
    }
    reportReady = true;
}

//  String / Number helpers

void HospitalManagementSystem::itoStr(int n, char* out) const {
    if (n == 0) { 
        out[0] = '0';
        out[1] = '\0';
        return;
    }
    if (n < 0) {
        out[0] = '-'; 
        itoStr(-n, out + 1);
        return; 
    }
    char tmp[24]; int i = 0;
    while (n > 0) { 
        tmp[i++] = '0' + n % 10;
        n /= 10; 
    }
    for (int j = 0; j < i; j++) {
        out[j] = tmp[i - 1 - j];
    }
    out[i] = '\0';
}

void HospitalManagementSystem::ftoStr(float f, char* out, int dec) const {
    if (f < 0.f) {
        out[0] = '-'; 
        ftoStr(-f, out + 1, dec);
        return; 
    }
    int whole = (int)f;
    itoStr(whole, out);
    int len2 = getLength(out);
    out[len2] = '.';
    float frac = f - (float)whole;
    for (int i = 0; i < dec; i++) {
        frac *= 10.f;
        out[len2 + 1 + i] = '0' + (int)frac;
        frac -= (int)frac;
    }
    out[len2 + 1 + dec] = '\0';
}

void HospitalManagementSystem::strApp(char* dst, const char* src) const {
    int di = 0;
    while (dst[di]) {
        di++;
    }
    int si = 0; 
    while (src[si]) {
        dst[di++] = src[si++];
    }
    dst[di] = '\0';
}

//  Date helpers

void HospitalManagementSystem::getTodayStr(char* out) const {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    int d = lt->tm_mday, mo = lt->tm_mon + 1, yr = lt->tm_year + 1900;
    out[0] = '0' + d / 10;
    out[1] = '0' + d % 10;
    out[2] = '-';
    out[3] = '0' + mo / 10; 
    out[4] = '0' + mo % 10;
    out[5] = '-';
    out[6] = '0' + yr / 1000;
    out[7] = '0' + (yr / 100) % 10;
    out[8] = '0' + (yr / 10) % 10;
    out[9] = '0' + yr % 10;
    out[10] = '\0';
}

int HospitalManagementSystem::dateKey(const char* d) const {
    if (!d || getLength(d) < 10) {
        return 0;
    }
    int dd = (d[0] - '0') * 10 + (d[1] - '0');
    int mm = (d[3] - '0') * 10 + (d[4] - '0');
    int yy = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
    return yy * 10000 + mm * 100 + dd;
}

int HospitalManagementSystem::cmpDate(const char* a, const char* b){
    int ka = dateKey(a), kb = dateKey(b);
    return ka < kb ? -1 : ka > kb ? 1 : 0;
}

bool HospitalManagementSystem::olderThan(const char* d, int days) {
    if (!d || getLength(d) < 10) {
        return false;
    }
    int dd = (d[0] - '0') * 10 + (d[1] - '0');
    int mm = (d[3] - '0') * 10 + (d[4] - '0');
    int yy = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
    tm dt = {};
    dt.tm_mday = dd; 
    dt.tm_mon = mm - 1; 
    dt.tm_year = yy - 1900;
    time_t t0 = mktime(&dt);
    time_t now = time(nullptr);
    return difftime(now, t0) / 86400.0 > days;
}

//  Storage helpers

int HospitalManagementSystem::nextApptID()  {
    int mx = 0;
    for (int i = 0; i < storApp.getSize(); i++) {
        if (storApp[i].getID() > mx) {
            mx = storApp[i].getID();
        }
    }
    return mx + 1;
}
int HospitalManagementSystem::nextBillID(){
    int mx = 0;
    for (int i = 0; i < storBill.getSize(); i++) {
        if (storBill[i].getID() > mx) mx = storBill[i].getID();
    }
    return mx + 1;
}
int HospitalManagementSystem::nextPresID(){
    int mx = 0;
    for (int i = 0; i < storPres.getSize(); i++) {
        if (storPres[i].getID() > mx) mx = storPres[i].getID();
    }
    return mx + 1;
}
int HospitalManagementSystem::nextDocID(){
    int mx = 0;
    for (int i = 0; i < storDoc.getSize(); i++) {
        if (storDoc[i].getID() > mx) mx = storDoc[i].getID();
    }
    return mx + 1;
}
bool HospitalManagementSystem::slotTaken(int docID, const char* date, const char* slot){
    for (int i = 0; i < storApp.getSize(); i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == docID && isEqual(a.getDate(), date) &&
            isEqual(a.getTimeSlot(), slot) && !isEqual(a.getStatus(), "cancelled")) {
            return true;
        }
    }
    return false;
}

//  Sort helpers  (bubble sort on index arrays)

void HospitalManagementSystem::sortApptsByDate(int* idx, int n, bool asc) {
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j < n - i - 1; j++) {
            int c = cmpDate(storApp[idx[j]].getDate(), storApp[idx[j + 1]].getDate());
            if ((asc && c > 0) || (!asc && c < 0))
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }
        }
}
}
void HospitalManagementSystem::sortApptsByTime(int* idx, int n) {
    for (int i = 0; i < n - 1; i++){
        for (int j = 0; j < n - i - 1; j++) {
            const char* ta = storApp[idx[j]].getTimeSlot();
            const char* tb = storApp[idx[j + 1]].getTimeSlot();
            bool sw = false;
            for (int k = 0; ta[k] && tb[k]; k++) {
                if (ta[k] > tb[k]) {
                    sw = true;
                    break;
                }
                if (ta[k] < tb[k]) {
                    break;
                }
            }
            if (sw) {
                int t = idx[j];
                idx[j] = idx[j + 1];
                idx[j + 1] = t;
            }
        }
}
}
void HospitalManagementSystem::sortPresByDate(int* idx, int n, bool asc){
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int c = cmpDate(storPres[idx[j]].getDate(), storPres[idx[j + 1]].getDate());
            if ((asc && c > 0) || (!asc && c < 0))
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }
        }
    }
}


//  Navigation

void HospitalManagementSystem::goTo(AppScreen s) {
    scr = s; 
    scrollY = 0;
    activeField = nullptr;
    for (int i = 0; i < 6; i++) {
        field[i].reset();
    }
    clrMsg();
    selApptID = selBillID = selDocID = selPatID = 0;

    if (s == SCR_LOGIN) { 
        fldID.reset();
        fldPwd.reset();
    }
    if (s == SCR_PAT_BOOK) { 
        bkStep = 0;
        bkSpec[0] = bkDate[0] = bkSlot[0] = '\0';
        bkDocID = 0; 
        bkDateFails = 0; 
    }
    if (s == SCR_DOC_PRESCRIBE) {
        prStep = 0;
        prApptID = 0;
    }
    if (s == SCR_DOC_HISTORY) {
        histSearched = false;
        histPatID = 0;
    }
    if (s == SCR_ADM_SECLOG) {
        loadSecLog(); 
    }
    if (s == SCR_ADM_REPORT) {
        reportReady = false;
        computeReport(); 
    }
}

void HospitalManagementSystem::logout() {
    role = ROLE_NONE; 
    logID = 0;
    failCount = 0; 
    locked = false;
    goTo(SCR_ROLE);
}

void HospitalManagementSystem::back() {
    switch (scr) {
    case SCR_LOGIN:         
        goTo(SCR_ROLE);     
        break;
    case SCR_PAT_MENU:      
        logout();            
        break;
    case SCR_PAT_BOOK:
    case SCR_PAT_CANCEL:
    case SCR_PAT_APPTS:
    case SCR_PAT_RECORDS:
    case SCR_PAT_BILLS:
    case SCR_PAT_PAY:
    case SCR_PAT_TOPUP:     
        goTo(SCR_PAT_MENU); 
        break;
    case SCR_DOC_MENU:      
        logout();            
        break;
    case SCR_DOC_TODAY:
    case SCR_DOC_COMPLETE:
    case SCR_DOC_NOSHOW:
    case SCR_DOC_PRESCRIBE:
    case SCR_DOC_HISTORY:   
        goTo(SCR_DOC_MENU); 
        break;
    case SCR_ADM_MENU:      
        logout();            
        break;
    default:                
        goTo(SCR_ADM_MENU);  
        break;
    }
}

//  UI primitives

sf::Text HospitalManagementSystem::mkTxt(const char* s, float x, float y, int sz, sf::Color c) const {
    sf::Text t;
    t.setFont(font);
    t.setString(s);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    t.setPosition(x, y);
    return t;
}

void HospitalManagementSystem::drawRect(float x, float y, float w, float h,
    sf::Color fill, sf::Color outline, float ot) {
    sf::RectangleShape r({ w, h });
    r.setPosition(x, y);
    r.setFillColor(fill);
    if (ot > 0.f) { 
        r.setOutlineColor(outline); 
        r.setOutlineThickness(ot);
    }
    win.draw(r);
}

bool HospitalManagementSystem::drawBtn(const char* lbl, float x, float y, float w, float h,sf::Color fill, sf::Vector2f mouse, sf::Color tc) {
    bool hov = btnHit(x, y, w, h, mouse);
    sf::Color c = hov ? sf::Color(
        fill.r + 30 > 255 ? 255 : fill.r + 30,
        fill.g + 30 > 255 ? 255 : fill.g + 30,
        fill.b + 30 > 255 ? 255 : fill.b + 30) : fill;
    drawRect(x, y, w, h, c, sf::Color(180, 215, 255, 55), 1.f);
    sf::Text t = mkTxt(lbl, 0, 0, 15, tc);
    sf::FloatRect tb = t.getLocalBounds();
    t.setPosition(x + (w - tb.width) * .5f, y + (h - 18) * .5f);
    win.draw(t);
    return hov;
}

bool HospitalManagementSystem::btnHit(float x, float y, float w, float h, sf::Vector2f p) const {
    return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
}

void HospitalManagementSystem::drawRow(float x, float y, float rh,
    const char** cells, const float* ws, int nc, sf::Color bg, sf::Color tc) {
    float totalW = 0; for (int i = 0; i < nc; i++) totalW += ws[i];
    drawRect(x, y, totalW, rh, bg);
    float cx = x;
    for (int i = 0; i < nc; i++) {
        win.draw(mkTxt(cells[i], cx + 8, y + (rh - 16) * .5f, 13, tc));
        cx += ws[i];
        drawRect(cx - 1, y, 1, rh, sf::Color(28, 42, 72));
    }
}

void HospitalManagementSystem::drawLbl(const char* text, float x, float y, sf::Color c) {
    win.draw(mkTxt(text, x, y, 15, c));
}

void HospitalManagementSystem::drawBg() {
    sf::RectangleShape hl({ (float)W, 1 });
    hl.setFillColor(sf::Color(20, 32, 55, 55));
    for (int yy = 0; yy < H; yy += 40) { 
        hl.setPosition(0, yy); 
        win.draw(hl); 
    }
    sf::RectangleShape vl({ 1, (float)H });
    vl.setFillColor(sf::Color(20, 32, 55, 55));
    for (int xx = 0; xx < W; xx += 40) {
        vl.setPosition(xx, 0);
        win.draw(vl); 
    }
}

void HospitalManagementSystem::drawHdr(const char* title, const char* sub) {
    drawRect(0, 0, W, HDR, sf::Color(8, 12, 22));
    drawRect(0, HDR - 2, W, 2, sf::Color(0, 155, 200, 210));

    win.draw(mkTxt("MediCore", 22, 13, 26, sf::Color(0, 200, 240)));
    win.draw(mkTxt("Hospital Management System", 22, 46, 12, sf::Color(85, 128, 172)));

    sf::Text ttl = mkTxt(title, 0, 14, 22, sf::Color(215, 232, 255));
    sf::FloatRect tb = ttl.getLocalBounds();
    ttl.setPosition((W - tb.width) * .5f, 14);
    win.draw(ttl);

    if (sub) {
        sf::Text st = mkTxt(sub, 0, 46, 12, sf::Color(0, 180, 218));
        sf::FloatRect sb = st.getLocalBounds();
        st.setPosition((W - sb.width) * .5f, 46);
        win.draw(st);
    }

    if (role != ROLE_NONE && logID != 0) {
        char info[128]; info[0] = '\0';
        if (role == ROLE_PAT) {
            Patient* p = storPat.findByID(logID);
            if (p) {
                strApp(info, p->getName());
                strApp(info, "  |  PKR ");
                char b[32];
                ftoStr(p->getBalance(), b);
                strApp(info, b);
            }
        }
        else if (role == ROLE_DOC) {
            Doctor* d = storDoc.findByID(logID);
            if (d) {
                strApp(info, "Dr. ");
                strApp(info, d->getName());
            }
        }
        else {
            strApp(info, "Administrator");
        }
        sf::Text ut = mkTxt(info, 0, 22, 13, sf::Color(160, 205, 255));
        sf::FloatRect ub = ut.getLocalBounds();
        ut.setPosition(W - ub.width - 18, 22);
        win.draw(ut);
    }
}

void HospitalManagementSystem::drawFtr() {
    drawRect(0, H - FTR, W, FTR, sf::Color(8, 12, 22));
    drawRect(0, H - FTR, W, 2, sf::Color(0, 100, 145, 160));
    win.draw(mkTxt("MediCore HMS  |  OOP Project  |  Spring 2026",
        22, H - FTR + 16, 12, sf::Color(48, 72, 108)));
    win.draw(mkTxt("ESC: Go Back   |   Mouse Wheel: Scroll",
        W - 320, H - FTR + 16, 12, sf::Color(48, 72, 108)));
}

void HospitalManagementSystem::drawMsg() {
    if (!msgOn) {
        return;
    }
    sf::Color bg = msgErr ? sf::Color(108, 16, 16) : sf::Color(12, 78, 28);
    drawRect(0, H - FTR - 36, W, 36, bg);
    win.draw(mkTxt(msgBuf, 20, H - FTR - 28, 14, sf::Color::White));
}

void HospitalManagementSystem::setMsg(const char* m, bool err) {
    copyStr(msgBuf, m); msgOn = true; msgErr = err;
}
void HospitalManagementSystem::clrMsg() { msgOn = false; msgBuf[0] = '\0'; }

//  Main click dispatcher

void HospitalManagementSystem::onClick(sf::Vector2f p) {
    switch (scr) {
    case SCR_ROLE:          
        onRoleClick(p);        
        break;
    case SCR_LOGIN:         
        onLoginClick(p);       
        break;
    case SCR_PAT_MENU:      
        onPatMenuClick(p);     
        break;
    case SCR_PAT_BOOK:      
        onPatBookClick(p);     
        break;
    case SCR_PAT_CANCEL:    
        onPatCancelClick(p);   
        break;
    case SCR_PAT_PAY:       
        onPatPayClick(p);      
        break;
    case SCR_PAT_TOPUP:     
        onPatTopupClick(p);    
        break;
    case SCR_DOC_MENU:      
        onDocMenuClick(p);     
        break;
    case SCR_DOC_COMPLETE:  
        onDocCompleteClick(p); 
        break;
    case SCR_DOC_NOSHOW:    
        onDocNoshowClick(p);   
        break;
    case SCR_DOC_PRESCRIBE: 
        onDocPrescribeClick(p); 
        break;
    case SCR_DOC_HISTORY:   
        onDocHistoryClick(p);  
        break;
    case SCR_ADM_MENU:      
        onAdmMenuClick(p);    
        break;
    case SCR_ADM_ADD_DOC:  
        onAdmAddDocClick(p);  
        break;
    case SCR_ADM_REM_DOC:  
        onAdmRemDocClick(p);  
        break;
    case SCR_ADM_DISCHARGE: 
        onAdmDischargeClick(p); 
        break;
        // back-only screens handled via ESC or Back button ,they are only read only
    case SCR_PAT_APPTS:
    case SCR_PAT_RECORDS:
    case SCR_PAT_BILLS:
    case SCR_DOC_TODAY:
    case SCR_ADM_PATIENTS:
    case SCR_ADM_DOCTORS:
    case SCR_ADM_APPTS:
    case SCR_ADM_UNPAID:
    case SCR_ADM_SECLOG:
    case SCR_ADM_REPORT:
        if (btnHit(40, H - HDR - FTR - 42, 90, 36, p)) {
            back();
        }
        break;
    default:
        break;
    }
}

//  SCR_ROLE

void HospitalManagementSystem::drawRole(sf::Vector2f m) {
    drawHdr("Welcome");

    sf::Text logo = mkTxt("MediCore", 0, 0, 58, sf::Color(0, 200, 242));
    sf::FloatRect lb = logo.getLocalBounds();
    logo.setPosition((W - lb.width) * .5f, 118);
    win.draw(logo);

    sf::Text sub = mkTxt("Hospital Management System", 0, 0, 20, sf::Color(128, 168, 215));
    sf::FloatRect sb = sub.getLocalBounds();
    sub.setPosition((W - sb.width) * .5f, 188);
    win.draw(sub);

    sf::Text hint = mkTxt("Select your role to continue", 0, 0, 14, sf::Color(78, 112, 162));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setPosition((W - hb.width) * .5f, 228);
    win.draw(hint);

    float bw = 300, bh = 62, bx = (W - bw) * .5f, by = 272, gap = 74;
    drawBtn("Patient", bx, by + 0 * gap, bw, bh, sf::Color(0, 100, 158), m);
    drawBtn("Doctor", bx, by + 1 * gap, bw, bh, sf::Color(68, 40, 162), m);
    drawBtn("Admin", bx, by + 2 * gap, bw, bh, sf::Color(128, 34, 34), m);
    drawBtn("Exit", bx, by + 3 * gap, bw, bh, sf::Color(38, 52, 78), m);
}

void HospitalManagementSystem::onRoleClick(sf::Vector2f p) {
    float bw = 300, bh = 62, bx = (W - bw) * .5f, by = 272, gap = 74;
    if (btnHit(bx, by + 0 * gap, bw, bh, p)) {
        role = ROLE_PAT; goTo(SCR_LOGIN);
    }
    else if (btnHit(bx, by + 1 * gap, bw, bh, p)) { 
        role = ROLE_DOC; goTo(SCR_LOGIN);
    }
    else if (btnHit(bx, by + 2 * gap, bw, bh, p)) {
        role = ROLE_ADM; goTo(SCR_LOGIN);
    }
    else if (btnHit(bx, by + 3 * gap, bw, bh, p)) {
        win.close();
    }
}


//  SCR_LOGIN


void HospitalManagementSystem::drawLogin(sf::Vector2f m) {
    const char* title = role == ROLE_PAT ? "Patient Login" :
        role == ROLE_DOC ? "Doctor Login" : "Admin Login";
    drawHdr(title);

    if (locked) {
        sf::Text lt = mkTxt("Account locked. Contact admin.", 0, 360, 22, sf::Color(222, 68, 68));
        sf::FloatRect lb = lt.getLocalBounds();
        lt.setPosition((W - lb.width) * .5f, 360);
        win.draw(lt);
        return;
    }

    drawRect(378, 250, 524, 320, sf::Color(13, 20, 40), sf::Color(0, 142, 182, 140), 2);

    drawLbl("User ID", 440, 294);
    fldID.setup(font, 440, 314, 400, 44); fldID.draw(win);

    drawLbl("Password", 440, 370);
    fldPwd.setup(font, 440, 390, 400, 44); fldPwd.draw(win);

    drawBtn("Login", 542, 450, 200, 46, sf::Color(0, 122, 178), m);
    drawBtn("Back", 440, 450, 88, 46, sf::Color(40, 55, 82), m);

    if (failCount > 0) {
        char fc[64] = "Failed attempts: "; char n[8]; itoStr(failCount, n); strApp(fc, n);
        sf::Text ft = mkTxt(fc, 0, 514, 13, sf::Color(215, 152, 58));
        sf::FloatRect fb = ft.getLocalBounds();
        ft.setPosition((W - fb.width) * .5f, 514);
        win.draw(ft);
    }
}

void HospitalManagementSystem::onLoginClick(sf::Vector2f p) {
    if (locked) {
        return;
    }
    if (fldID.hit(p)) { 
        fldID.active = true; 
        fldPwd.active = false;
        activeField = &fldID; 
        return;
    }
    if (fldPwd.hit(p)) {
        fldPwd.active = true;
        fldID.active = false; 
        activeField = &fldPwd; 
        return; 
    }
    if (btnHit(440, 450, 88, 46, p)) {
        back();
        return; 
    }
    if (btnHit(542, 450, 200, 46, p)) {
        int id = (int)stringToFloat(fldID.buf);
        if (!Validator::validateID(id)) {
            setMsg("Please enter a valid numeric ID.");
            return; 
        }
        if (tryLogin(id, fldPwd.buf)) {
            logID = id;
            failCount = 0;
            clrMsg();
            if (role == ROLE_PAT) {
                goTo(SCR_PAT_MENU);
            }
            else if (role == ROLE_DOC) {
                goTo(SCR_DOC_MENU);
            }
            else {
                goTo(SCR_ADM_MENU);
            }
        }
        else {
            failCount++;
            const char* rs = role == ROLE_PAT ? "Patient" : role == ROLE_DOC ? "Doctor" : "Admin";
            f.writeSecurityLog(rs, id, "FAILED");
            if (failCount >= 3) {
                locked = true;
                setMsg("Account locked. Contact admin.");
            }
            else {
                setMsg("Invalid ID or password. Try again.");
            }
        }
    }
}

bool HospitalManagementSystem::tryLogin(int id, const char* pwd) {
    if (role == ROLE_PAT) { 
        Patient* p = storPat.findByID(id);
        return p && isEqual(p->getPass(), pwd);
    }
    if (role == ROLE_DOC) {
        Doctor* d = storDoc.findByID(id); 
        return d && isEqual(d->getPass(), pwd);
    }
    Admin* a = storAdm.findByID(id);
    return a && isEqual(a->getPass(), pwd);
}


//  SCR_PATIENT_MENU


void HospitalManagementSystem::drawPatMenu(sf::Vector2f m) {
    Patient* p = storPat.findByID(logID);
    char sub[128] = "Balance: PKR ";
    if (p) {
        char b[32];
        ftoStr(p->getBalance(), b); 
        strApp(sub, b); 
}
    drawHdr(p ? p->getName() : "Patient Dashboard", sub);

    float bw = 620, bh = 52, bx = (W - bw) * .5f, by = 102, gap = 60;
    const char* lbls[] = {
        "1.  Book Appointment",
        "2.  Cancel Appointment",
        "3.  View My Appointments",
        "4.  View My Medical Records",
        "5.  View My Bills",
        "6.  Pay Bill",
        "7.  Top Up Balance",
        "8.  Logout"
    };
    sf::Color cols[] = {
        sf::Color(0,102,158), sf::Color(145,36,36), sf::Color(0,88,138),
        sf::Color(52,90,155), sf::Color(62,62,148), sf::Color(0,115,75),
        sf::Color(115,88,0),  sf::Color(40,55,78)
    };
    for (int i = 0; i < 8; i++) {
        drawBtn(lbls[i], bx, by + i * gap, bw, bh, cols[i], m);
    }
}

void HospitalManagementSystem::onPatMenuClick(sf::Vector2f p) {
    float bw = 620, bh = 52, bx = (W - bw) * .5f, by = 102, gap = 60;
    AppScreen scrs[] = { SCR_PAT_BOOK, SCR_PAT_CANCEL, SCR_PAT_APPTS, SCR_PAT_RECORDS,
                         SCR_PAT_BILLS, SCR_PAT_PAY, SCR_PAT_TOPUP };
    for (int i = 0; i < 7; i++)
        if (btnHit(bx, by + i * gap, bw, bh, p)) {
            goTo(scrs[i]);
            return;
        }
    if (btnHit(bx, by + 7 * gap, bw, bh, p)) {
        logout();
    }
}


//  SCR_PATIENT_BOOKING 

void HospitalManagementSystem::drawPatBook(sf::Vector2f m) {
    drawHdr("Book Appointment", "Find a doctor and schedule your visit");
    float lx = 40, cy = CY + 8;

    if (bkStep == 0) {
        drawLbl("Enter Specialization to Search:", lx, cy);
        field[0].setup(font, lx, cy + 26, 480, 44);
        field[0].draw(win);
        drawBtn("Search", lx + 490, cy + 26, 160, 44, sf::Color(0, 118, 172), m);
        win.draw(mkTxt("e.g. Cardiology, Neurology, Orthopedics", lx, cy + 80, 13, sf::Color(75, 110, 158)));
        drawBtn("Back", lx, cy + 100, 90, 38, sf::Color(38, 52, 78), m);
    }
    else if (bkStep == 1) {
        char stitle[96] = "Doctors in: ";
        strApp(stitle, bkSpec);
        win.draw(mkTxt(stitle, lx, cy, 17, sf::Color(0, 198, 232)));

        const char* hdrs[] = { "ID","Name","Specialization","Fee (PKR)" };
        const float ws[] = { 65.f, 280.f, 285.f, 155.f };
        drawRow(lx, cy + 24, 30, hdrs, ws, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

        int row = 0;
        for (int i = 0; i < storDoc.getSize(); i++) {
            Doctor& d = storDoc[i];
            char ds[64], bs[64];
            copyStr(ds, d.getSpecialization()); 
            copyStr(bs, bkSpec);
            convertToLower(ds); 
            convertToLower(bs);
            if (!isEqual(ds, bs)) {
                continue;
            }
            char sid[16], sfee[32];
            itoStr(d.getID(), sid);
            ftoStr(d.getFee(), sfee);
            bool sel = (bkDocID == d.getID());
            sf::Color bg = sel ? sf::Color(0, 90, 55) : (row % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
            const char* cells[] = { sid, d.getName(), d.getSpecialization(), sfee };
            drawRow(lx, cy + 54 + row * 34, 34, cells, ws, 4, bg);
            row++;
        }
        if (row == 0)
            win.draw(mkTxt("No doctors found for that specialization.", lx, cy + 68, 15, sf::Color(215, 100, 100)));

        float ay = cy + 54 + row * 34 + 14;
        drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
        if (bkDocID > 0) {
            drawBtn("Continue", lx + 100, ay, 140, 38, sf::Color(0, 122, 80), m);
        }
        if (bkDocID > 0) {
            char sel[80] = "Selected: ";
            Doctor* dd = storDoc.findByID(bkDocID);
            if (dd) { 
                strApp(sel, dd->getName());
            }
            win.draw(mkTxt(sel, lx + 252, ay + 10, 13, sf::Color(0, 222, 132)));
        }
    }
    else if (bkStep == 2) {
        Doctor* d = storDoc.findByID(bkDocID);
        char dtitle[128] = "Scheduling with: ";
        if (d) { strApp(dtitle, d->getName()); strApp(dtitle, " ("); strApp(dtitle, d->getSpecialization()); strApp(dtitle, ")"); }
        win.draw(mkTxt(dtitle, lx, cy, 17, sf::Color(162, 202, 248)));
        drawLbl("Enter Date (DD-MM-YYYY):", lx, cy + 40);
        field[1].setup(font, lx, cy + 62, 260, 44); field[1].draw(win);
        drawBtn("Check Available Slots", lx + 270, cy + 62, 240, 44, sf::Color(0, 118, 172), m);
        win.draw(mkTxt("e.g. 15-06-2026  |  Must be today or a future date", lx, cy + 116, 13, sf::Color(75, 110, 155)));
        drawBtn("Back", lx, cy + 138, 90, 38, sf::Color(38, 52, 78), m);
    }
    else if (bkStep == 3) {
        Doctor* d = storDoc.findByID(bkDocID);
        char info[128] = ""; if (d) { strApp(info, d->getName()); strApp(info, "  |  "); strApp(info, bkDate); }
        win.draw(mkTxt(info, lx, cy, 16, sf::Color(162, 202, 248)));
        drawLbl("Available Time Slots — click to select:", lx, cy + 30);

        const char* slots[] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
        float sx = lx, sy = cy + 56, sw = 145, sh = 55, sgap = 11;
        for (int i = 0; i < 8; i++) {
            bool taken = slotTaken(bkDocID, bkDate, slots[i]);
            bool sel = isEqual(bkSlot, slots[i]);
            sf::Color c = taken ? sf::Color(72, 22, 22) : (sel ? sf::Color(0, 118, 65) : sf::Color(0, 75, 128));
            sf::Color tc = taken ? sf::Color(148, 78, 78) : sf::Color::White;
            float bx2 = sx + (i % 4) * (sw + sgap), by2 = sy + (i / 4) * (sh + sgap);
            drawRect(bx2, by2, sw, sh, c, sf::Color(178, 212, 255, 52), 1);
            sf::Text st = mkTxt(taken ? "TAKEN" : slots[i], 0, 0, 15, tc);
            sf::FloatRect sb = st.getLocalBounds();
            st.setPosition(bx2 + (sw - sb.width) * .5f, by2 + (sh - 18) * .5f);
            win.draw(st);
        }
        float actY = sy + 2 * (sh + sgap) + 10;
        if (bkSlot[0] != '\0') {
            char conf[96] = "Slot: "; strApp(conf, bkSlot);
            if (d) { char f2[32]; strApp(conf, "  |  Fee: PKR "); ftoStr(d->getFee(), f2); strApp(conf, f2); }
            win.draw(mkTxt(conf, lx, actY, 15, sf::Color(0, 228, 132)));
            drawBtn("Confirm & Book", lx, actY + 28, 220, 46, sf::Color(0, 132, 78), m);
            drawBtn("Back", lx + 230, actY + 28, 90, 38, sf::Color(38, 52, 78), m);
        }
        else {
            drawBtn("Back", lx, actY + 28, 90, 38, sf::Color(38, 52, 78), m);
        }
    }
}

void HospitalManagementSystem::onPatBookClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;

    if (bkStep == 0) {
        if (field[0].hit(p)) { field[0].active = true; activeField = &field[0]; return; }
        if (btnHit(lx, cy + 100, 90, 38, p)) { back(); return; }
        if (btnHit(lx + 490, cy + 26, 160, 44, p)) {
            if (getLength(field[0].buf) == 0) { setMsg("Please enter a specialization."); return; }
            copyStr(bkSpec, field[0].buf);
            bool found = false;
            for (int i = 0; i < storDoc.getSize(); i++) {
                char ds[64], bs[64];
                copyStr(ds, storDoc[i].getSpecialization()); copyStr(bs, bkSpec);
                convertToLower(ds); convertToLower(bs);
                if (isEqual(ds, bs)) { found = true; break; }
            }
            if (!found) { setMsg("No doctors available for that specialization."); return; }
            bkStep = 1; clrMsg();
        }
    }
    else if (bkStep == 1) {
        int row = 0;
        for (int i = 0; i < storDoc.getSize(); i++) {
            Doctor& d = storDoc[i];
            char ds[64], bs[64];
            copyStr(ds, d.getSpecialization()); copyStr(bs, bkSpec);
            convertToLower(ds); convertToLower(bs);
            if (!isEqual(ds, bs)) continue;
            float ry2 = cy + 54 + row * 34;
            if (btnHit(lx, ry2, 785, 34, p)) { bkDocID = d.getID(); return; }
            row++;
        }
        float ay = cy + 54 + row * 34 + 14;
        if (btnHit(lx, ay, 90, 38, p)) { bkStep = 0; bkDocID = 0; return; }
        if (bkDocID > 0 && btnHit(lx + 100, ay, 140, 38, p)) { bkStep = 2; clrMsg(); }
    }
    else if (bkStep == 2) {
        if (field[1].hit(p)) { field[1].active = true; activeField = &field[1]; return; }
        if (btnHit(lx, cy + 138, 90, 38, p)) { bkStep = 1; return; }
        if (btnHit(lx + 270, cy + 62, 240, 44, p)) {
            if (!Validator::validateDate(field[1].buf)) {
                bkDateFails++;
                if (bkDateFails >= 3) { setMsg("Too many invalid attempts. Returning to menu."); goTo(SCR_PAT_MENU); return; }
                setMsg("Invalid date. Use DD-MM-YYYY. Must be today or a future date.");
                return;
            }
            copyStr(bkDate, field[1].buf);
            bkSlot[0] = '\0'; bkStep = 3; clrMsg();
        }
    }
    else if (bkStep == 3) {
        const char* slots[] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
        float sx = lx, sy = CY + 8 + 56, sw = 145, sh = 55, sgap = 11;
        for (int i = 0; i < 8; i++) {
            float bx2 = sx + (i % 4) * (sw + sgap), by2 = sy + (i / 4) * (sh + sgap);
            if (btnHit(bx2, by2, sw, sh, p)) {
                if (slotTaken(bkDocID, bkDate, slots[i])) {
                    try { throw SlotUnavailableException(); }
                    catch (SlotUnavailableException& e) { setMsg(e.what()); }
                }
                else { copyStr(bkSlot, slots[i]); clrMsg(); }
                return;
            }
        }
        float actY = sy + 2 * (sh + sgap) + 10;
        if (bkSlot[0] != '\0' && btnHit(lx, actY + 28, 220, 46, p)) { doBookAppt(); return; }
        float backX = bkSlot[0] != '\0' ? lx + 230 : lx;
        if (btnHit(backX, actY + 28, 90, 38, p)) { bkStep = 2; }
    }
}

void HospitalManagementSystem::doBookAppt() {
    Patient* pat = storPat.findByID(logID);
    Doctor* doc = storDoc.findByID(bkDocID);
    if (!pat || !doc) { setMsg("Internal error: patient or doctor not found."); return; }
    try { *pat -= doc->getFee(); }
    catch (InsufficientFundsException& e) { setMsg(e.what()); return; }

    char today[16]; getTodayStr(today);
    int aid = nextApptID(), bid = nextBillID();
    Appointment a(aid, logID, bkDocID, bkDate, bkSlot, "pending");
    storApp.add(a); f.appendAppointment(a);
    Bill b(bid, logID, aid, doc->getFee(), "unpaid", today);
    storBill.add(b); f.appendBill(b);
    f.savePatients(storPat);

    char sm[128] = "Appointment booked! ID: ";
    char sid[16]; itoStr(aid, sid); strApp(sm, sid);
    setMsg(sm, false);
    goTo(SCR_PAT_MENU);
}


//  SCR_PATIENT_CANCEL


void HospitalManagementSystem::drawPatCancel(sf::Vector2f m) {
    drawHdr("Cancel Appointment");
    float lx = 40, cy = CY + 8;
    drawLbl("Pending Appointments — click a row to select:", lx, cy);

    const char* hdrs[] = { "ID","Doctor","Specialization","Date","Slot","Status" };
    const float ws[] = { 65.f,210.f,200.f,120.f,80.f,110.f };
    drawRow(lx, cy + 24, 30, hdrs, ws, 6, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getPatID() == logID && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByDate(idx, n, true);

    for (int ii = 0; ii < n; ii++) {
        Appointment& a = storApp[idx[ii]];
        Doctor* d = storDoc.findByID(a.getDocID());
        char sid[16]; itoStr(a.getID(), sid);
        bool sel = (selApptID == a.getID());
        sf::Color bg = sel ? sf::Color(0, 88, 52) : (ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
        const char* cells[] = { sid,d ? d->getName() : "?",d ? d->getSpecialization() : "?",a.getDate(),a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 54 + ii * 34, 34, cells, ws, 6, bg);
    }
    if (n == 0) win.draw(mkTxt("You have no pending appointments.", lx, cy + 68, 15, sf::Color(202, 145, 95)));

    float ay = cy + 54 + n * 34 + 14;
    drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
    if (selApptID > 0) {
        char lbl[64] = "Cancel Appointment #"; char sid[16]; itoStr(selApptID, sid); strApp(lbl, sid);
        drawBtn(lbl, lx + 100, ay, 300, 40, sf::Color(145, 30, 30), m);
        Appointment* sa = storApp.findByID(selApptID);
        if (sa) {
            Doctor* sd = storDoc.findByID(sa->getDocID());
            if (sd) {
                char rf[64] = "Refund: PKR "; char f2[32]; ftoStr(sd->getFee(), f2); strApp(rf, f2);
                win.draw(mkTxt(rf, lx + 412, ay + 10, 13, sf::Color(0, 220, 132)));
            }
        }
    }
}

void HospitalManagementSystem::onPatCancelClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getPatID() == logID && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByDate(idx, n, true);
    for (int ii = 0; ii < n; ii++) {
        if (btnHit(lx, cy + 54 + ii * 34, 785, 34, p)) { selApptID = storApp[idx[ii]].getID(); return; }
    }
    float ay = cy + 54 + n * 34 + 14;
    if (btnHit(lx, ay, 90, 38, p)) { back(); return; }
    if (selApptID > 0 && btnHit(lx + 100, ay, 300, 40, p)) doCancelAppt(selApptID);
}

void HospitalManagementSystem::doCancelAppt(int apptID) {
    Appointment* a = storApp.findByID(apptID);
    if (!a) { setMsg("Appointment not found."); return; }
    Doctor* d = storDoc.findByID(a->getDocID());
    float fee = d ? d->getFee() : 0.f;
    a->setStatus("cancelled"); f.saveAppointment(storApp);
    for (int i = 0; i < storBill.getSize(); i++)
        if (storBill[i].getAppointmentID() == apptID) { storBill[i].setStatus("cancelled"); break; }
    f.saveBills(storBill);
    Patient* pat = storPat.findByID(logID);
    if (pat) { *pat += fee; f.savePatients(storPat); }
    char sm[128] = "Appointment cancelled. PKR "; char f2[32]; ftoStr(fee, f2);
    strApp(sm, f2); strApp(sm, " refunded.");
    selApptID = 0; setMsg(sm, false);
}


//  SCR_PATIENT_APPTS

void HospitalManagementSystem::drawPatAppts(sf::Vector2f m) {
    drawHdr("My Appointments", "Sorted by date ascending");
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++)
        if (storApp[i].getPatID() == logID) idx[n++] = i;
    sortApptsByDate(idx, n, true);

    const char* hdrs[] = { "ID","Doctor","Specialization","Date","Slot","Status" };
    const float ws[] = { 65.f,205.f,205.f,120.f,80.f,110.f };
    drawRow(lx, cy, 30, hdrs, ws, 6, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    int startRow = scrollY / rowH;
    for (int ii = startRow; ii < n && ii - startRow < maxRows; ii++) {
        Appointment& a = storApp[idx[ii]];
        Doctor* d = storDoc.findByID(a.getDocID());
        char sid[16]; itoStr(a.getID(), sid);
        sf::Color tc = isEqual(a.getStatus(), "completed") ? sf::Color(0, 205, 122) :
            isEqual(a.getStatus(), "cancelled") ? sf::Color(182, 82, 82) :
            isEqual(a.getStatus(), "no-show") ? sf::Color(202, 152, 50) : sf::Color(215, 232, 255);
        sf::Color bg = ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,d ? d->getName() : "?",d ? d->getSpecialization() : "?",a.getDate(),a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 30 + (ii - startRow) * rowH, rowH, cells, ws, 6, bg, tc);
    }
    if (n == 0) win.draw(mkTxt("No appointments found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));

    float ay = H - HDR - FTR - 40;
    drawBtn("Back", lx, ay, 90, 36, sf::Color(38, 52, 78), m);
    char cnt[64] = "Total: "; char nc[8]; itoStr(n, nc); strApp(cnt, nc); strApp(cnt, " appointments");
    win.draw(mkTxt(cnt, lx + 108, ay + 10, 13, sf::Color(125, 162, 210)));
}

//  SCR_PATIENT_RECORDS

void HospitalManagementSystem::drawPatRecords(sf::Vector2f m) {
    drawHdr("My Medical Records", "Sorted by date — most recent first");
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storPres.getSize() && n < 100; i++)
        if (storPres[i].getPatientID() == logID) idx[n++] = i;
    sortPresByDate(idx, n, false);

    if (n == 0) {
        win.draw(mkTxt("No medical records found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));
        drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m); return;
    }

    int cardH = 88, maxCards = (H - HDR - FTR - 60 - cy) / cardH;
    int startCard = scrollY / cardH;
    for (int ii = startCard; ii < n && ii - startCard < maxCards; ii++) {
        Prescription& pr = storPres[idx[ii]];
        Doctor* d = storDoc.findByID(pr.getDoctorID());
        float ry = cy + (ii - startCard) * cardH;
        sf::Color bg = ii % 2 ? sf::Color(16, 25, 46) : sf::Color(20, 32, 55);
        drawRect(lx, ry, 1185, cardH - 4, bg, sf::Color(0, 105, 142, 80), 1);
        char l1[256] = "Date: "; strApp(l1, pr.getDate()); strApp(l1, "   |   Dr. ");
        if (d) strApp(l1, d->getName()); else strApp(l1, "Unknown");
        win.draw(mkTxt(l1, lx + 10, ry + 7, 13, sf::Color(0, 202, 235)));
        char l2[512] = "Medicines: "; strApp(l2, pr.getMedicines());
        win.draw(mkTxt(l2, lx + 10, ry + 30, 12, sf::Color(215, 232, 255)));
        char l3[512] = "Notes: "; strApp(l3, pr.getNotes());
        win.draw(mkTxt(l3, lx + 10, ry + 52, 12, sf::Color(168, 192, 235)));
    }
    float ay = H - HDR - FTR - 40;
    drawBtn("Back", lx, ay, 90, 36, sf::Color(38, 52, 78), m);
    char cnt[64] = "Total: "; char nc[8]; itoStr(n, nc); strApp(cnt, nc); strApp(cnt, " records");
    win.draw(mkTxt(cnt, lx + 108, ay + 10, 13, sf::Color(125, 162, 210)));
}


//  SCR_PATIENT_BILLS

void HospitalManagementSystem::drawPatBills(sf::Vector2f m) {
    drawHdr("My Bills");
    float lx = 40, cy = CY + 8;

    const char* hdrs[] = { "Bill ID","Appt ID","Amount (PKR)","Status","Date" };
    const float ws[] = { 88.f,88.f,160.f,120.f,128.f };
    drawRow(lx, cy, 30, hdrs, ws, 5, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    float unpaid = 0.f; int row = 0;
    int rowH = 34, maxRows = (H - HDR - FTR - 90 - cy - 30) / rowH;
    int startRow = scrollY / rowH;
    for (int i = 0; i < storBill.getSize(); i++) {
        Bill& b = storBill[i];
        if (b.getPatientID() != logID) continue;
        if (isEqual(b.getStatus(), "unpaid")) unpaid += b.getAmount();
        if (row < startRow) { row++; continue; }
        if (row - startRow >= maxRows) { row++; continue; }
        char sid[16], sapp[16], samt[32];
        itoStr(b.getID(), sid); itoStr(b.getAppointmentID(), sapp); ftoStr(b.getAmount(), samt);
        sf::Color tc = isEqual(b.getStatus(), "paid") ? sf::Color(0, 205, 122) :
            isEqual(b.getStatus(), "cancelled") ? sf::Color(182, 82, 82) : sf::Color(222, 158, 42);
        sf::Color bg = row % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,sapp,samt,b.getStatus(),b.getDate() };
        drawRow(lx, cy + 30 + (row - startRow) * rowH, rowH, cells, ws, 5, bg, tc);
        row++;
    }
    if (row == 0) win.draw(mkTxt("No bills found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));

    char tot[64] = "Total Unpaid: PKR "; char tv[32]; ftoStr(unpaid, tv); strApp(tot, tv);
    win.draw(mkTxt(tot, lx, H - HDR - FTR - 68, 15, unpaid > 0.f ? sf::Color(225, 152, 45) : sf::Color(0, 212, 122)));
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}

//  SCR_PAT_PAY

void HospitalManagementSystem::drawPatPay(sf::Vector2f m) {
    drawHdr("Pay Bill");
    float lx = 40, cy = CY + 8;
    drawLbl("Unpaid Bills — click a row to select:", lx, cy);

    const char* hdrs[] = { "Bill ID","Appt ID","Amount (PKR)","Date" };
    const float ws[] = { 88.f,88.f,160.f,128.f };
    drawRow(lx, cy + 24, 30, hdrs, ws, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int idx[100]; int n = 0;
    for (int i = 0; i < storBill.getSize() && n < 100; i++)
        if (storBill[i].getPatientID() == logID && isEqual(storBill[i].getStatus(), "unpaid")) idx[n++] = i;

    if (n == 0) {
        win.draw(mkTxt("No unpaid bills.", lx, cy + 68, 15, sf::Color(0, 212, 122)));
        drawBtn("Back", lx, cy + 106, 90, 38, sf::Color(38, 52, 78), m); return;
    }
    for (int ii = 0; ii < n; ii++) {
        Bill& b = storBill[idx[ii]];
        char sid[16], sapp[16], samt[32];
        itoStr(b.getID(), sid); itoStr(b.getAppointmentID(), sapp); ftoStr(b.getAmount(), samt);
        bool sel = (selBillID == b.getID());
        sf::Color bg = sel ? sf::Color(0, 88, 52) : (ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
        const char* cells[] = { sid,sapp,samt,b.getDate() };
        drawRow(lx, cy + 54 + ii * 34, 34, cells, ws, 4, bg);
    }
    float ay = cy + 54 + n * 34 + 14;
    drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
    if (selBillID > 0) {
        char lbl[64] = "Pay Bill #"; char sid[16]; itoStr(selBillID, sid); strApp(lbl, sid);
        drawBtn(lbl, lx + 100, ay, 200, 40, sf::Color(0, 122, 78), m);
        Bill* sb = storBill.findByID(selBillID);
        if (sb) {
            char af[64] = "Amount: PKR "; char amt[32]; ftoStr(sb->getAmount(), amt); strApp(af, amt);
            win.draw(mkTxt(af, lx + 315, ay + 10, 13, sf::Color(0, 222, 132)));
        }
    }
}

void HospitalManagementSystem::onPatPayClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    int idx[100]; int n = 0;
    for (int i = 0; i < storBill.getSize() && n < 100; i++)
        if (storBill[i].getPatientID() == logID && isEqual(storBill[i].getStatus(), "unpaid")) idx[n++] = i;
    if (n == 0) { if (btnHit(lx, cy + 106, 90, 38, p)) back(); return; }
    for (int ii = 0; ii < n; ii++)
        if (btnHit(lx, cy + 54 + ii * 34, 464, 34, p)) { selBillID = storBill[idx[ii]].getID(); return; }
    float ay = cy + 54 + n * 34 + 14;
    if (btnHit(lx, ay, 90, 38, p)) { back(); return; }
    if (selBillID > 0 && btnHit(lx + 100, ay, 200, 40, p)) doPayBill(selBillID);
}

void HospitalManagementSystem::doPayBill(int billID) {
    Bill* b = storBill.findByID(billID);
    if (!b) { setMsg("Bill not found."); return; }
    Patient* pat = storPat.findByID(logID);
    if (!pat) { setMsg("Patient not found."); return; }
    try { *pat -= b->getAmount(); }
    catch (InsufficientFundsException& e) { setMsg(e.what()); return; }
    b->setStatus("paid");
    f.saveBills(storBill); f.savePatients(storPat);
    char sm[128] = "Bill paid. Remaining balance: PKR ";
    char bal[32]; ftoStr(pat->getBalance(), bal); strApp(sm, bal);
    selBillID = 0; setMsg(sm, false);
}

//  SCR_PAT_TOPUP

void HospitalManagementSystem::drawPatTopup(sf::Vector2f m) {
    drawHdr("Top Up Balance");
    float px = 378, py = 272;
    drawRect(px, py, 524, 200, sf::Color(13, 20, 40), sf::Color(0, 142, 182, 140), 2);
    Patient* pat = storPat.findByID(logID);
    if (pat) {
        char b[64] = "Current Balance: PKR "; char bv[32]; ftoStr(pat->getBalance(), bv); strApp(b, bv);
        win.draw(mkTxt(b, px + 20, py + 15, 15, sf::Color(0, 212, 132)));
    }
    drawLbl("Amount to Add (PKR):", px + 20, py + 48);
    field[0].setup(font, px + 20, py + 70, 400, 44); field[0].draw(win);
    drawBtn("Add Balance", px + 20, py + 128, 175, 44, sf::Color(0, 118, 78), m);
    drawBtn("Back", px + 206, py + 128, 90, 44, sf::Color(38, 52, 78), m);
}

void HospitalManagementSystem::onPatTopupClick(sf::Vector2f p) {
    float px = 378, py = 272;
    if (field[0].hit(p)) { field[0].active = true; activeField = &field[0]; return; }
    if (btnHit(px + 206, py + 128, 90, 44, p)) { back(); return; }
    if (btnHit(px + 20, py + 128, 175, 44, p)) {
        float amt = stringToFloat(field[0].buf);
        try { if (!Validator::validatePositiveFloat(amt)) throw InvalidInputException("Amount must be greater than 0."); }
        catch (InvalidInputException& e) { setMsg(e.what()); return; }
        doTopup(amt);
    }
}

void HospitalManagementSystem::doTopup(float amt) {
    Patient* pat = storPat.findByID(logID);
    if (!pat) { setMsg("Patient not found."); return; }
    *pat += amt; f.savePatients(storPat);
    char sm[128] = "Balance updated. New balance: PKR ";
    char bv[32]; ftoStr(pat->getBalance(), bv); strApp(sm, bv);
    field[0].reset(); setMsg(sm, false);
}

//  SCR_DOC_MENU

void HospitalManagementSystem::drawDocMenu(sf::Vector2f m) {
    Doctor* d = storDoc.findByID(logID);
    char sub[128] = "Specialization: ";
    if (d) strApp(sub, d->getSpecialization());
    drawHdr(d ? "Doctor Dashboard" : "Doctor Dashboard", sub);

    if (d) {
        char nm[80] = "Welcome, Dr. "; strApp(nm, d->getName());
        sf::Text wt = mkTxt(nm, 0, 0, 20, sf::Color(0, 200, 240));
        sf::FloatRect wb = wt.getLocalBounds();
        wt.setPosition((W - wb.width) * .5f, CY);
        win.draw(wt);
    }

    float bw = 620, bh = 52, bx = (W - bw) * .5f, by = 128, gap = 66;
    const char* lbls[] = {
        "1.  View Today's Appointments",
        "2.  Mark Appointment Complete",
        "3.  Mark Appointment No-Show",
        "4.  Write Prescription",
        "5.  View Patient Medical History",
        "6.  Logout"
    };
    sf::Color cols[] = {
        sf::Color(0,102,158),sf::Color(0,115,78),sf::Color(115,78,0),
        sf::Color(52,90,155),sf::Color(62,62,148),sf::Color(40,55,78)
    };
    for (int i = 0; i < 6; i++) drawBtn(lbls[i], bx, by + i * gap, bw, bh, cols[i], m);
}

void HospitalManagementSystem::onDocMenuClick(sf::Vector2f p) {
    float bw = 620, bh = 52, bx = (W - bw) * .5f, by = 128, gap = 66;
    AppScreen scrs[] = { SCR_DOC_TODAY,SCR_DOC_COMPLETE,SCR_DOC_NOSHOW,SCR_DOC_PRESCRIBE,SCR_DOC_HISTORY };
    for (int i = 0; i < 5; i++) if (btnHit(bx, by + i * gap, bw, bh, p)) { goTo(scrs[i]); return; }
    if (btnHit(bx, by + 5 * gap, bw, bh, p)) logout();
}

//  SCR_DOC_TODAY

void HospitalManagementSystem::drawDocToday(sf::Vector2f m) {
    char today[16]; getTodayStr(today);
    char sub[64] = "Date: "; strApp(sub, today);
    drawHdr("Today's Appointments", sub);
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == logID && isEqual(a.getDate(), today)) idx[n++] = i;
    }
    sortApptsByTime(idx, n);

    const char* hdrs[] = { "ID","Patient","Date","Slot","Status" };
    const float ws[] = { 65.f,240.f,130.f,88.f,120.f };
    drawRow(lx, cy, 30, hdrs, ws, 5, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    for (int ii = 0; ii < n && ii < maxRows; ii++) {
        Appointment& a = storApp[idx[ii]];
        Patient* pt = storPat.findByID(a.getPatID());
        char sid[16]; itoStr(a.getID(), sid);
        sf::Color tc = isEqual(a.getStatus(), "completed") ? sf::Color(0, 205, 122) :
            isEqual(a.getStatus(), "no-show") ? sf::Color(202, 152, 50) : sf::Color(215, 232, 255);
        sf::Color bg = ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,pt ? pt->getName() : "?",a.getDate(),a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 30 + ii * rowH, rowH, cells, ws, 5, bg, tc);
    }
    if (n == 0) win.draw(mkTxt("No appointments scheduled for today.", lx, cy + 50, 15, sf::Color(202, 145, 95)));

    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
    char cnt[64] = "Total today: "; char nc[8]; itoStr(n, nc); strApp(cnt, nc);
    win.draw(mkTxt(cnt, lx + 108, H - HDR - FTR - 32, 13, sf::Color(125, 162, 210)));
}

//  SCR_DOC_COMPLETE

void HospitalManagementSystem::drawDocComplete(sf::Vector2f m) {
    char today[16]; getTodayStr(today);
    drawHdr("Mark Appointment Complete", "Click a pending appointment");
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == logID && isEqual(a.getDate(), today) && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByTime(idx, n);

    const char* hdrs[] = { "ID","Patient","Slot","Status" };
    const float ws[] = { 65.f,280.f,100.f,120.f };
    drawRow(lx, cy, 30, hdrs, ws, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    for (int ii = 0; ii < n; ii++) {
        Appointment& a = storApp[idx[ii]];
        Patient* pt = storPat.findByID(a.getPatID());
        char sid[16]; itoStr(a.getID(), sid);
        bool sel = (selApptID == a.getID());
        sf::Color bg = sel ? sf::Color(0, 88, 52) : (ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
        const char* cells[] = { sid,pt ? pt->getName() : "?",a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 30 + ii * 34, 34, cells, ws, 4, bg);
    }
    if (n == 0) win.draw(mkTxt("No pending appointments for today.", lx, cy + 50, 15, sf::Color(202, 145, 95)));

    float ay = cy + 30 + n * 34 + 14;
    drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
    if (selApptID > 0) drawBtn("Mark as Completed", lx + 100, ay, 240, 40, sf::Color(0, 122, 78), m);
}

void HospitalManagementSystem::onDocCompleteClick(sf::Vector2f p) {
    char today[16]; getTodayStr(today);
    float lx = 40, cy = CY + 8;
    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == logID && isEqual(a.getDate(), today) && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByTime(idx, n);
    for (int ii = 0; ii < n; ii++)
        if (btnHit(lx, cy + 30 + ii * 34, 565, 34, p)) { selApptID = storApp[idx[ii]].getID(); return; }
    float ay = cy + 30 + n * 34 + 14;
    if (btnHit(lx, ay, 90, 38, p)) { back(); return; }
    if (selApptID > 0 && btnHit(lx + 100, ay, 240, 40, p)) doMarkComplete(selApptID);
}

void HospitalManagementSystem::doMarkComplete(int apptID) {
    Appointment* a = storApp.findByID(apptID);
    if (!a) { setMsg("Appointment not found."); return; }
    a->setStatus("completed"); f.saveAppointment(storApp);
    selApptID = 0; setMsg("Appointment marked as completed.", false);
}

//  SCR_DOC_NOSHOW

void HospitalManagementSystem::drawDocNoshow(sf::Vector2f m) {
    char today[16]; getTodayStr(today);
    drawHdr("Mark Appointment No-Show", "Click a pending appointment");
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == logID && isEqual(a.getDate(), today) && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByTime(idx, n);

    const char* hdrs[] = { "ID","Patient","Slot","Status" };
    const float ws[] = { 65.f,280.f,100.f,120.f };
    drawRow(lx, cy, 30, hdrs, ws, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    for (int ii = 0; ii < n; ii++) {
        Appointment& a = storApp[idx[ii]];
        Patient* pt = storPat.findByID(a.getPatID());
        char sid[16]; itoStr(a.getID(), sid);
        bool sel = (selApptID == a.getID());
        sf::Color bg = sel ? sf::Color(90, 55, 0) : (ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
        const char* cells[] = { sid,pt ? pt->getName() : "?",a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 30 + ii * 34, 34, cells, ws, 4, bg);
    }
    if (n == 0) win.draw(mkTxt("No pending appointments for today.", lx, cy + 50, 15, sf::Color(202, 145, 95)));

    float ay = cy + 30 + n * 34 + 14;
    drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
    if (selApptID > 0) drawBtn("Mark as No-Show", lx + 100, ay, 240, 40, sf::Color(132, 80, 0), m);
}

void HospitalManagementSystem::onDocNoshowClick(sf::Vector2f p) {
    char today[16]; getTodayStr(today);
    float lx = 40, cy = CY + 8;
    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == logID && isEqual(a.getDate(), today) && isEqual(a.getStatus(), "pending")) idx[n++] = i;
    }
    sortApptsByTime(idx, n);
    for (int ii = 0; ii < n; ii++)
        if (btnHit(lx, cy + 30 + ii * 34, 565, 34, p)) { selApptID = storApp[idx[ii]].getID(); return; }
    float ay = cy + 30 + n * 34 + 14;
    if (btnHit(lx, ay, 90, 38, p)) { back(); return; }
    if (selApptID > 0 && btnHit(lx + 100, ay, 240, 40, p)) doMarkNoshow(selApptID);
}

void HospitalManagementSystem::doMarkNoshow(int apptID) {
    Appointment* a = storApp.findByID(apptID);
    if (!a) { setMsg("Appointment not found."); return; }
    a->setStatus("no-show"); f.saveAppointment(storApp);
    for (int i = 0; i < storBill.getSize(); i++)
        if (storBill[i].getAppointmentID() == apptID) { storBill[i].setStatus("cancelled"); break; }
    f.saveBills(storBill);
    selApptID = 0; setMsg("Appointment marked as no-show.", false);
}

//  SCR_DOC_PRESCRIBE 

void HospitalManagementSystem::drawDocPrescribe(sf::Vector2f m) {
    drawHdr("Write Prescription");
    float lx = 40, cy = CY + 8;

    if (prStep == 0) {
        drawLbl("Enter Appointment ID (must be completed, by you):", lx, cy);
        field[0].setup(font, lx, cy + 26, 280, 44); field[0].draw(win);
        drawBtn("Find", lx + 290, cy + 26, 120, 44, sf::Color(0, 118, 172), m);
        drawBtn("Back", lx, cy + 84, 90, 38, sf::Color(38, 52, 78), m);
    }
    else {
        Appointment* a = storApp.findByID(prApptID);
        Patient* pt = storPat.findByID(a ? a->getPatID() : 0);
        char info[128] = "Appointment #"; char aid[16]; itoStr(prApptID, aid); strApp(info, aid);
        if (pt) { strApp(info, "  |  Patient: "); strApp(info, pt->getName()); }
        win.draw(mkTxt(info, lx, cy, 15, sf::Color(0, 202, 238)));

        drawLbl("Medicines (separate with ;):", lx, cy + 32);
        field[1].setup(font, lx, cy + 52, 900, 44); field[1].draw(win);

        drawLbl("Notes:", lx, cy + 108);
        field[2].setup(font, lx, cy + 128, 900, 44); field[2].draw(win);

        drawBtn("Save Prescription", lx, cy + 188, 240, 46, sf::Color(0, 118, 78), m);
        drawBtn("Back", lx + 250, cy + 188, 90, 46, sf::Color(38, 52, 78), m);

        win.draw(mkTxt("e.g. Medicines: Paracetamol 500mg;Amoxicillin 250mg", lx, cy + 244, 12, sf::Color(72, 105, 152)));
    }
}

void HospitalManagementSystem::onDocPrescribeClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    if (prStep == 0) {
        if (field[0].hit(p)) { field[0].active = true; activeField = &field[0]; return; }
        if (btnHit(lx, cy + 84, 90, 38, p)) { back(); return; }
        if (btnHit(lx + 290, cy + 26, 120, 44, p)) {
            int aid = (int)stringToFloat(field[0].buf);
            Appointment* a = storApp.findByID(aid);
            if (!a || a->getDocID() != logID || !isEqual(a->getStatus(), "completed")) {
                setMsg("Invalid: appointment must be completed and belong to you."); return;
            }
            for (int i = 0; i < storPres.getSize(); i++)
                if (storPres[i].getAppointmentID() == aid) { setMsg("Prescription already written for this appointment."); return; }
            prApptID = aid; prStep = 1; clrMsg();
        }
    }
    else {
        if (field[1].hit(p)) { field[1].active = true; field[2].active = false; activeField = &field[1]; return; }
        if (field[2].hit(p)) { field[2].active = true; field[1].active = false; activeField = &field[2]; return; }
        if (btnHit(lx + 250, cy + 188, 90, 46, p)) { prStep = 0; return; }
        if (btnHit(lx, cy + 188, 240, 46, p)) doWritePrescription();
    }
}

void HospitalManagementSystem::doWritePrescription() {
    if (getLength(field[1].buf) == 0) { setMsg("Please enter medicines."); return; }
    Appointment* a = storApp.findByID(prApptID);
    if (!a) { setMsg("Appointment not found."); return; }
    char today[16]; getTodayStr(today);
    int pid = nextPresID();
    Prescription pr(pid, prApptID, a->getPatID(), logID, today, field[1].buf, field[2].len > 0 ? field[2].buf : "N/A");
    storPres.add(pr); f.appendPrescription(pr);
    setMsg("Prescription saved successfully.", false);
    goTo(SCR_DOC_MENU);
}

//  SCR_DOC_HISTORY

void HospitalManagementSystem::drawDocHistory(sf::Vector2f m) {
    drawHdr("Patient Medical History");
    float lx = 40, cy = CY + 8;

    drawLbl("Enter Patient ID:", lx, cy);
    field[0].setup(font, lx, cy + 24, 240, 44); field[0].draw(win);
    drawBtn("Search", lx + 250, cy + 24, 120, 44, sf::Color(0, 118, 172), m);
    drawBtn("Back", lx, cy + 82, 90, 38, sf::Color(38, 52, 78), m);

    if (!histSearched) return;

    // Validate: patient must have at least one completed appt with this doctor
    bool hasAppt = false;
    for (int i = 0; i < storApp.getSize(); i++) {
        Appointment& a = storApp[i];
        if (a.getPatID() == histPatID && a.getDocID() == logID && isEqual(a.getStatus(), "completed")) { hasAppt = true; break; }
    }
    if (!hasAppt) { win.draw(mkTxt("Access denied. No completed appointments with this patient.", lx, cy + 130, 14, sf::Color(215, 82, 82))); return; }

    Patient* pt = storPat.findByID(histPatID);
    char pname[80] = "Patient: "; if (pt) strApp(pname, pt->getName());
    win.draw(mkTxt(pname, lx, cy + 130, 16, sf::Color(0, 202, 238)));

    int idx[100]; int n = 0;
    for (int i = 0; i < storPres.getSize() && n < 100; i++) {
        Prescription& pr = storPres[i];
        if (pr.getPatientID() == histPatID && pr.getDoctorID() == logID) idx[n++] = i;
    }
    sortPresByDate(idx, n, false);

    if (n == 0) { win.draw(mkTxt("No prescriptions found for this patient.", lx, cy + 158, 14, sf::Color(202, 145, 95))); return; }

    int cardH = 80, maxCards = (H - HDR - FTR - 60 - (cy + 158)) / cardH;
    int startCard = scrollY / cardH;
    for (int ii = startCard; ii < n && ii - startCard < maxCards; ii++) {
        Prescription& pr = storPres[idx[ii]];
        float ry = cy + 158 + (ii - startCard) * cardH;
        sf::Color bg = ii % 2 ? sf::Color(16, 25, 46) : sf::Color(20, 32, 55);
        drawRect(lx, ry, 1185, cardH - 4, bg, sf::Color(0, 105, 142, 78), 1);
        char l1[256] = "Date: "; strApp(l1, pr.getDate());
        win.draw(mkTxt(l1, lx + 10, ry + 6, 13, sf::Color(0, 202, 235)));
        char l2[512] = "Medicines: "; strApp(l2, pr.getMedicines());
        win.draw(mkTxt(l2, lx + 10, ry + 28, 12, sf::Color(215, 232, 255)));
        char l3[512] = "Notes: "; strApp(l3, pr.getNotes());
        win.draw(mkTxt(l3, lx + 10, ry + 50, 12, sf::Color(168, 192, 235)));
    }
}

void HospitalManagementSystem::onDocHistoryClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    if (field[0].hit(p)) { field[0].active = true; activeField = &field[0]; return; }
    if (btnHit(lx, cy + 82, 90, 38, p)) { back(); return; }
    if (btnHit(lx + 250, cy + 24, 120, 44, p)) {
        int id = (int)stringToFloat(field[0].buf);
        if (!Validator::validateID(id) || !storPat.findByID(id)) { setMsg("Patient not found."); return; }
        histPatID = id; histSearched = true; clrMsg();
    }
}

//  SCR_ADM_MENU

void HospitalManagementSystem::drawAdmMenu(sf::Vector2f m) {
    drawHdr("Admin Panel — MediCore");
    float bw = 660, bh = 48, bx = (W - bw) * .5f, by = 96, gap = 56;
    const char* lbls[] = {
        "1.  Add Doctor",
        "2.  Remove Doctor",
        "3.  View All Patients",
        "4.  View All Doctors",
        "5.  View All Appointments",
        "6.  View Unpaid Bills",
        "7.  Discharge Patient",
        "8.  View Security Log",
        "9.  Generate Daily Report",
        "10. Logout"
    };
    sf::Color cols[] = {
        sf::Color(0,105,162),sf::Color(145,36,36),sf::Color(0,88,138),
        sf::Color(52,90,155),sf::Color(62,62,148),sf::Color(0,115,75),
        sf::Color(115,88,0),sf::Color(48,65,95),sf::Color(62,95,62),sf::Color(40,55,78)
    };
    for (int i = 0; i < 10; i++) drawBtn(lbls[i], bx, by + i * gap, bw, bh, cols[i], m);
}

void HospitalManagementSystem::onAdmMenuClick(sf::Vector2f p) {
    float bw = 660, bh = 48, bx = (W - bw) * .5f, by = 96, gap = 56;
    AppScreen scrs[] = { SCR_ADM_ADD_DOC,SCR_ADM_REM_DOC,SCR_ADM_PATIENTS,SCR_ADM_DOCTORS,
                      SCR_ADM_APPTS,SCR_ADM_UNPAID,SCR_ADM_DISCHARGE,SCR_ADM_SECLOG,SCR_ADM_REPORT };
    for (int i = 0; i < 9; i++) if (btnHit(bx, by + i * gap, bw, bh, p)) { goTo(scrs[i]); return; }
    if (btnHit(bx, by + 9 * gap, bw, bh, p)) logout();
}

//  SCR_ADM_ADD_DOC

void HospitalManagementSystem::drawAdmAddDoc(sf::Vector2f m) {
    drawHdr("Add Doctor");
    float lx = 40, cy = CY + 8;
    const char* lbls[] = { "Name (max 50 chars):","Specialization:","Contact (11 digits):","Password (min 6 chars):","Consultation Fee (PKR):" };
    for (int i = 0; i < 5; i++) {
        drawLbl(lbls[i], lx, cy + i * 68);
        field[i].setup(font, lx, cy + 20 + i * 68, 500, 40);
        field[i].draw(win);
    }
    drawBtn("Add Doctor", lx, cy + 5 * 68 + 8, 180, 46, sf::Color(0, 118, 78), m);
    drawBtn("Back", lx + 190, cy + 5 * 68 + 8, 90, 46, sf::Color(38, 52, 78), m);
}

void HospitalManagementSystem::onAdmAddDocClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    for (int i = 0; i < 5; i++)
        if (field[i].hit(p)) {
            for (int j = 0; j < 5; j++) field[j].active = false;
            field[i].active = true; activeField = &field[i]; return;
        }
    if (btnHit(lx + 190, cy + 5 * 68 + 8, 90, 46, p)) { back(); return; }
    if (btnHit(lx, cy + 5 * 68 + 8, 180, 46, p)) doAddDoctor();
}

void HospitalManagementSystem::doAddDoctor() {
    if (getLength(field[0].buf) == 0) { setMsg("Name cannot be empty."); return; }
    if (getLength(field[1].buf) == 0) { setMsg("Specialization cannot be empty."); return; }
    if (!Validator::validateContact(field[2].buf)) { setMsg("Contact must be exactly 11 digits."); return; }
    if (!Validator::validatePassword(field[3].buf)) { setMsg("Password must be at least 6 characters."); return; }
    float fee = stringToFloat(field[4].buf);
    if (!Validator::validatePositiveFloat(fee)) { setMsg("Fee must be a positive number."); return; }

    int newID = nextDocID();
    char sid[16]; itoStr(newID, sid);
    Doctor d(field[1].buf, fee, newID, field[0].buf, field[2].buf, field[3].buf);
    storDoc.add(d); f.appendDoctor(d);

    char sm[64] = "Doctor added successfully. ID: "; strApp(sm, sid);
    setMsg(sm, false);
    goTo(SCR_ADM_MENU);
}

//  SCR_ADM_REM_DOC

void HospitalManagementSystem::drawAdmRemDoc(sf::Vector2f m) {
    drawHdr("Remove Doctor");
    float lx = 40, cy = CY + 8;
    drawLbl("All Doctors — click a row to select:", lx, cy);

    const char* hdrs[] = { "ID","Name","Specialization","Contact","Fee" };
    const float ws[] = { 65.f,220.f,220.f,150.f,110.f };
    drawRow(lx, cy + 24, 30, hdrs, ws, 5, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 120 - cy - 54) / rowH;
    int startRow = scrollY / rowH;
    for (int i = startRow; i < storDoc.getSize() && i - startRow < maxRows; i++) {
        Doctor& d = storDoc[i];
        char sid[16], sfee[32]; itoStr(d.getID(), sid); ftoStr(d.getFee(), sfee);
        bool sel = (selDocID == d.getID());
        sf::Color bg = sel ? sf::Color(90, 20, 20) : (i % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60));
        const char* cells[] = { sid,d.getName(),d.getSpecialization(),d.getContact(),sfee };
        drawRow(lx, cy + 54 + (i - startRow) * rowH, rowH, cells, ws, 5, bg);
    }

    float ay = H - HDR - FTR - 48;
    drawBtn("Back", lx, ay, 90, 38, sf::Color(38, 52, 78), m);
    if (selDocID > 0) {
        char lbl[64] = "Remove Doctor #"; char sid[16]; itoStr(selDocID, sid); strApp(lbl, sid);
        drawBtn(lbl, lx + 100, ay, 240, 40, sf::Color(145, 30, 30), m);
    }
}

void HospitalManagementSystem::onAdmRemDocClick(sf::Vector2f p) {
    float lx = 40, cy = CY + 8;
    int rowH = 34, maxRows = (H - HDR - FTR - 120 - cy - 54) / rowH;
    int startRow = scrollY / rowH;
    for (int i = startRow; i < storDoc.getSize() && i - startRow < maxRows; i++) {
        float ry = cy + 54 + (i - startRow) * rowH;
        if (btnHit(lx, ry, 765, rowH, p)) { selDocID = storDoc[i].getID(); return; }
    }
    float ay = H - HDR - FTR - 48;
    if (btnHit(lx, ay, 90, 38, p)) { back(); return; }
    if (selDocID > 0 && btnHit(lx + 100, ay, 240, 40, p)) doRemoveDoctor(selDocID);
}

void HospitalManagementSystem::doRemoveDoctor(int docID) {
    for (int i = 0; i < storApp.getSize(); i++) {
        Appointment& a = storApp[i];
        if (a.getDocID() == docID && isEqual(a.getStatus(), "pending")) {
            setMsg("Cannot remove doctor with pending appointments."); return;
        }
    }
    storDoc.removeByID(docID); f.saveDoctor(storDoc);
    char sm[64] = "Doctor removed successfully.";
    selDocID = 0; setMsg(sm, false);
}

//  SCR_ADM_PATIENTS

void HospitalManagementSystem::drawAdmPatients(sf::Vector2f m) {
    drawHdr("All Patients");
    float lx = 40, cy = CY + 8;

    const char* hdrs[] = { "ID","Name","Age","G","Contact","Balance","Unpaid Bills" };
    const float ws[] = { 65.f,195.f,50.f,35.f,140.f,120.f,105.f };
    drawRow(lx, cy, 30, hdrs, ws, 7, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    int startRow = scrollY / rowH;
    for (int i = startRow; i < storPat.getSize() && i - startRow < maxRows; i++) {
        Patient& p = storPat[i];
        char sid[16], sage[8], sbal[32], subills[8];
        itoStr(p.getID(), sid); itoStr(p.getAge(), sage); ftoStr(p.getBalance(), sbal);
        char g[2] = { (char)p.getGender(),'\0' };
        int ub = 0;
        for (int j = 0; j < storBill.getSize(); j++)
            if (storBill[j].getPatientID() == p.getID() && isEqual(storBill[j].getStatus(), "unpaid")) ub++;
        itoStr(ub, subills);
        sf::Color bg = i % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,p.getName(),sage,g,p.getContact(),sbal,subills };
        drawRow(lx, cy + 30 + (i - startRow) * rowH, rowH, cells, ws, 7, bg);
    }
    if (storPat.getSize() == 0) win.draw(mkTxt("No patients found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}

//  SCR_ADM_DOCTORS

void HospitalManagementSystem::drawAdmDoctors(sf::Vector2f m) {
    drawHdr("All Doctors");
    float lx = 40, cy = CY + 8;

    const char* hdrs[] = { "ID","Name","Specialization","Contact","Fee (PKR)" };
    const float ws[] = { 65.f,215.f,220.f,150.f,130.f };
    drawRow(lx, cy, 30, hdrs, ws, 5, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    int startRow = scrollY / rowH;
    for (int i = startRow; i < storDoc.getSize() && i - startRow < maxRows; i++) {
        Doctor& d = storDoc[i];
        char sid[16], sfee[32]; itoStr(d.getID(), sid); ftoStr(d.getFee(), sfee);
        sf::Color bg = i % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,d.getName(),d.getSpecialization(),d.getContact(),sfee };
        drawRow(lx, cy + 30 + (i - startRow) * rowH, rowH, cells, ws, 5, bg);
    }
    if (storDoc.getSize() == 0) win.draw(mkTxt("No doctors found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}

//  SCR_ADM_APPTS

void HospitalManagementSystem::drawAdmAppts(sf::Vector2f m) {
    drawHdr("All Appointments", "Sorted by date descending");
    float lx = 40, cy = CY + 8;

    int idx[100]; int n = 0;
    for (int i = 0; i < storApp.getSize() && n < 100; i++) idx[n++] = i;
    sortApptsByDate(idx, n, false);

    const char* hdrs[] = { "ID","Patient","Doctor","Date","Slot","Status" };
    const float ws[] = { 65.f,185.f,185.f,120.f,80.f,110.f };
    drawRow(lx, cy, 30, hdrs, ws, 6, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    int startRow = scrollY / rowH;
    for (int ii = startRow; ii < n && ii - startRow < maxRows; ii++) {
        Appointment& a = storApp[idx[ii]];
        Patient* pt = storPat.findByID(a.getPatID());
        Doctor* d = storDoc.findByID(a.getDocID());
        char sid[16]; itoStr(a.getID(), sid);
        sf::Color tc = isEqual(a.getStatus(), "completed") ? sf::Color(0, 205, 122) :
            isEqual(a.getStatus(), "cancelled") ? sf::Color(182, 82, 82) :
            isEqual(a.getStatus(), "no-show") ? sf::Color(202, 152, 50) : sf::Color(215, 232, 255);
        sf::Color bg = ii % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,pt ? pt->getName() : "?",d ? d->getName() : "?",a.getDate(),a.getTimeSlot(),a.getStatus() };
        drawRow(lx, cy + 30 + (ii - startRow) * rowH, rowH, cells, ws, 6, bg, tc);
    }
    if (n == 0) win.draw(mkTxt("No appointments found.", lx, cy + 50, 15, sf::Color(202, 145, 95)));
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}

//  SCR_ADM_UNPAID

void HospitalManagementSystem::drawAdmUnpaid(sf::Vector2f m) {
    drawHdr("Unpaid Bills", "Bills older than 7 days are marked OVERDUE");
    float lx = 40, cy = CY + 8;

    const char* hdrs[] = { "Bill ID","Patient","Amount (PKR)","Date" };
    const float ws[] = { 88.f,230.f,160.f,210.f };
    drawRow(lx, cy, 30, hdrs, ws, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));

    int rowH = 34, maxRows = (H - HDR - FTR - 60 - cy - 30) / rowH;
    int startRow = scrollY / rowH, row = 0;
    for (int i = 0; i < storBill.getSize(); i++) {
        Bill& b = storBill[i];
        if (!isEqual(b.getStatus(), "unpaid")) continue;
        if (row < startRow) { row++; continue; }
        if (row - startRow >= maxRows) { row++; continue; }
        Patient* pt = storPat.findByID(b.getPatientID());
        char sid[16], samt[32]; itoStr(b.getID(), sid); ftoStr(b.getAmount(), samt);
        char datecol[64]; copyStr(datecol, b.getDate());
        if (olderThan(b.getDate(), 7)) strApp(datecol, " [OVERDUE]");
        sf::Color tc = olderThan(b.getDate(), 7) ? sf::Color(225, 82, 82) : sf::Color(222, 158, 42);
        sf::Color bg = row % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { sid,pt ? pt->getName() : "?",samt,datecol };
        drawRow(lx, cy + 30 + (row - startRow) * rowH, rowH, cells, ws, 4, bg, tc);
        row++;
    }
    if (row == 0) win.draw(mkTxt("No unpaid bills.", lx, cy + 50, 15, sf::Color(0, 212, 122)));
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}

//  SCR_ADM_DISCHARGE

void HospitalManagementSystem::drawAdmDischarge(sf::Vector2f m) {
    drawHdr("Discharge Patient");
    float px = 378, py = 260;
    drawRect(px, py, 524, 240, sf::Color(13, 20, 40), sf::Color(0, 142, 182, 140), 2);
    drawLbl("Patient will be archived to discharged.txt", px + 20, py + 15, sf::Color(0, 202, 238));
    drawLbl("(Patient must have no unpaid bills or pending appointments)", px + 20, py + 38, sf::Color(72, 100, 145));
    drawLbl("Enter Patient ID:", px + 20, py + 70);
    field[0].setup(font, px + 20, py + 92, 380, 44); field[0].draw(win);
    drawBtn("Discharge Patient", px + 20, py + 152, 220, 46, sf::Color(132, 30, 30), m);
    drawBtn("Back", px + 250, py + 152, 90, 46, sf::Color(38, 52, 78), m);
}

void HospitalManagementSystem::onAdmDischargeClick(sf::Vector2f p) {
    float px = 378, py = 260;
    if (field[0].hit(p)) { field[0].active = true; activeField = &field[0]; return; }
    if (btnHit(px + 250, py + 152, 90, 46, p)) { back(); return; }
    if (btnHit(px + 20, py + 152, 220, 46, p)) {
        int id = (int)stringToFloat(field[0].buf);
        if (!Validator::validateID(id) || !storPat.findByID(id)) { setMsg("Patient not found."); return; }
        doDischarge(id);
    }
}

void HospitalManagementSystem::doDischarge(int patID) {
    for (int i = 0; i < storBill.getSize(); i++)
        if (storBill[i].getPatientID() == patID && isEqual(storBill[i].getStatus(), "unpaid")) {
            setMsg("Cannot discharge patient with unpaid bills."); return;
        }
    for (int i = 0; i < storApp.getSize(); i++)
        if (storApp[i].getPatID() == patID && isEqual(storApp[i].getStatus(), "pending")) {
            setMsg("Cannot discharge patient with pending appointments."); return;
        }
    f.dischargePatient(patID, storPat, storApp, storBill, storPres);
    field[0].reset(); setMsg("Patient discharged and archived successfully.", false);
}

//  SCR_ADM_SECLOG

void HospitalManagementSystem::drawAdmSeclog(sf::Vector2f m) {
    drawHdr("Security Log");
    float lx = 40, cy = CY + 8;

    if (secCount == 0) {
        win.draw(mkTxt("No security events logged.", lx, cy + 50, 15, sf::Color(202, 145, 95)));
        drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m); return;
    }
    int rowH = 28, maxRows = (H - HDR - FTR - 60 - cy) / rowH;
    int startRow = scrollY / rowH;
    for (int i = startRow; i < secCount && i - startRow < maxRows; i++) {
        sf::Color bg = i % 2 ? sf::Color(16, 24, 44) : sf::Color(20, 30, 52);
        drawRect(lx, cy + (i - startRow) * rowH, 1185, rowH, bg);
        sf::Color tc = getLength(secLines[i]) > 0 && secLines[i][getLength(secLines[i]) - 1] == 'D' ?
            sf::Color(215, 82, 82) : sf::Color(188, 210, 245);
        win.draw(mkTxt(secLines[i], lx + 8, cy + (i - startRow) * rowH + 6, 13, tc));
    }
    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
    char cnt[64] = "Entries: "; char nc[8]; itoStr(secCount, nc); strApp(cnt, nc);
    win.draw(mkTxt(cnt, lx + 108, H - HDR - FTR - 32, 13, sf::Color(125, 162, 210)));
}

//  SCR_ADM_REPORT

void HospitalManagementSystem::drawAdmReport(sf::Vector2f m) {
    char today[16]; getTodayStr(today);
    char sub[64] = "Report for: "; strApp(sub, today);
    drawHdr("Daily Report", sub);
    float lx = 40, cy = CY + 12;

    //Appointments summary
    drawRect(lx, cy, 600, 140, sf::Color(14, 22, 42), sf::Color(0, 140, 182, 100), 1);
    win.draw(mkTxt("Appointments Today", lx + 12, cy + 8, 16, sf::Color(0, 202, 238)));
    char at[64] = "Total: ";   char n1[8]; itoStr(rptTotal, n1); strApp(at, n1);
    char ap[64] = "Pending: "; char n2[8]; itoStr(rptPend, n2);  strApp(ap, n2);
    char ac[64] = "Completed: "; char n3[8]; itoStr(rptComp, n3); strApp(ac, n3);
    char ans[64] = "No-show: "; char n4[8]; itoStr(rptNos, n4);  strApp(ans, n4);
    char aca[64] = "Cancelled: "; char n5[8]; itoStr(rptCanc, n5); strApp(aca, n5);
    win.draw(mkTxt(at, lx + 12, cy + 36, 14, sf::Color(215, 232, 255)));
    win.draw(mkTxt(ap, lx + 12, cy + 58, 14, sf::Color(222, 158, 42)));
    win.draw(mkTxt(ac, lx + 12, cy + 80, 14, sf::Color(0, 205, 122)));
    win.draw(mkTxt(ans, lx + 12, cy + 102, 14, sf::Color(202, 152, 50)));
    win.draw(mkTxt(aca, lx + 12, cy + 124, 14, sf::Color(182, 82, 82)));

    //Revenue
    drawRect(lx + 620, cy, 600, 140, sf::Color(14, 22, 42), sf::Color(0, 140, 182, 100), 1);
    win.draw(mkTxt("Revenue Today (Paid Bills)", lx + 632, cy + 8, 16, sf::Color(0, 202, 238)));
    char rev[64] = "PKR "; char rv[32]; ftoStr(rptRev, rv); strApp(rev, rv);
    win.draw(mkTxt(rev, lx + 632, cy + 50, 22, sf::Color(0, 220, 132)));

    //Patients with unpaid bills
    drawRect(lx, cy + 155, 600, 300, sf::Color(14, 22, 42), sf::Color(0, 140, 182, 100), 1);
    win.draw(mkTxt("Patients with Unpaid Bills", lx + 12, cy + 163, 15, sf::Color(0, 202, 238)));
    const char* hdrs2[] = { "Patient Name","Total Owed" };
    const float ws2[] = { 330.f,130.f };
    drawRow(lx + 12, cy + 188, 26, hdrs2, ws2, 2, sf::Color(0, 75, 115), sf::Color(0, 202, 238));
    int row2 = 0;
    for (int i = 0; i < storPat.getSize() && row2 < 8; i++) {
        Patient& p = storPat[i];
        float owed = 0.f;
        for (int j = 0; j < storBill.getSize(); j++)
            if (storBill[j].getPatientID() == p.getID() && isEqual(storBill[j].getStatus(), "unpaid"))
                owed += storBill[j].getAmount();
        if (owed == 0.f) continue;
        char sowed[32]; ftoStr(owed, sowed);
        sf::Color bg = row2 % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { p.getName(),sowed };
        drawRow(lx + 12, cy + 214 + row2 * 26, 26, cells, ws2, 2, bg, sf::Color(222, 158, 42));
        row2++;
    }
    if (row2 == 0) win.draw(mkTxt("None", lx + 12, cy + 220, 14, sf::Color(0, 212, 122)));

    // Doctor-wise summary 
    drawRect(lx + 620, cy + 155, 600, 300, sf::Color(14, 22, 42), sf::Color(0, 140, 182, 100), 1);
    win.draw(mkTxt("Doctor Summary Today", lx + 632, cy + 163, 15, sf::Color(0, 202, 238)));
    const char* hdrs3[] = { "Doctor","Comp","Pend","N/S" };
    const float ws3[] = { 260.f,60.f,60.f,60.f };
    drawRow(lx + 632, cy + 188, 26, hdrs3, ws3, 4, sf::Color(0, 75, 115), sf::Color(0, 202, 238));
    int row3 = 0;
    for (int i = 0; i < storDoc.getSize() && row3 < 8; i++) {
        Doctor& d = storDoc[i];
        int dc = 0, dp = 0, dn = 0;
        for (int j = 0; j < storApp.getSize(); j++) {
            Appointment& a = storApp[j];
            if (a.getDocID() != d.getID() || !isEqual(a.getDate(), today)) continue;
            if (isEqual(a.getStatus(), "completed")) dc++;
            else if (isEqual(a.getStatus(), "pending")) dp++;
            else if (isEqual(a.getStatus(), "no-show")) dn++;
        }
        char sc[8], sp[8], sn[8]; itoStr(dc, sc); itoStr(dp, sp); itoStr(dn, sn);
        sf::Color bg = row3 % 2 ? sf::Color(18, 27, 50) : sf::Color(22, 34, 60);
        const char* cells[] = { d.getName(),sc,sp,sn };
        drawRow(lx + 632, cy + 214 + row3 * 26, 26, cells, ws3, 4, bg);
        row3++;
    }
    if (row3 == 0) win.draw(mkTxt("No doctor activity today.", lx + 632, cy + 220, 14, sf::Color(202, 145, 95)));

    drawBtn("Back", lx, H - HDR - FTR - 40, 90, 36, sf::Color(38, 52, 78), m);
}