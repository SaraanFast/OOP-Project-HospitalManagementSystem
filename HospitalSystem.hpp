#ifndef HMS_HPP
#define HMS_HPP

#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include "Storage.hpp"
#include "Patient.hpp"
#include "Doctor.hpp"
#include "Admin.hpp"
#include "Appointment.hpp"
#include "Bill.hpp"
#include "Prescription.hpp"
#include "FileHandler.hpp"
#include "Validator.hpp"
#include "HospitalException.hpp"
#include "helperFunctions.hpp"

//  Screen identifiers
enum AppScreen {
    SCR_ROLE = 0,
    SCR_LOGIN,
    SCR_PAT_MENU,
    SCR_PAT_BOOK,
    SCR_PAT_CANCEL,
    SCR_PAT_APPTS,
    SCR_PAT_RECORDS,
    SCR_PAT_BILLS,
    SCR_PAT_PAY,
    SCR_PAT_TOPUP,
    SCR_DOC_MENU,
    SCR_DOC_TODAY,
    SCR_DOC_COMPLETE,
    SCR_DOC_NOSHOW,
    SCR_DOC_PRESCRIBE,
    SCR_DOC_HISTORY,
    SCR_ADM_MENU,
    SCR_ADM_ADD_DOC,
    SCR_ADM_REM_DOC,
    SCR_ADM_PATIENTS,
    SCR_ADM_DOCTORS,
    SCR_ADM_APPTS,
    SCR_ADM_UNPAID,
    SCR_ADM_DISCHARGE,
    SCR_ADM_SECLOG,
    SCR_ADM_REPORT
};
enum UserRole { ROLE_NONE = 0, ROLE_PAT, ROLE_DOC, ROLE_ADM };

//  Simple input-field widget
struct InputField {
    sf::RectangleShape box;
    sf::Text           txt;
    char               buf[512];
    int                len;
    bool               active;
    bool               isPwd;

    InputField() : len(0), active(false), isPwd(false) { buf[0] = '\0'; }
    void reset() { buf[0] = '\0'; len = 0; active = false; }
    void push(char c) { if (len < 510) { buf[len++] = c; buf[len] = '\0'; } }
    void pop() { if (len > 0) buf[--len] = '\0'; }

    void setup(sf::Font& f, float x, float y, float w, float h, int fs = 17);
    void draw(sf::RenderWindow& w);
    bool hit(sf::Vector2f p) const;
};

class HospitalManagementSystem {

    /* ── data stores ──────────────────────────────────────── */
    Storage<Patient>      storPat;
    Storage<Doctor>       storDoc;
    Storage<Appointment>  storApp;
    Storage<Admin>        storAdm;
    Storage<Bill>         storBill;
    Storage<Prescription> storPres;
    File                  fh;

    /* ── SFML ─────────────────────────────────────────────── */
    sf::RenderWindow win;
    sf::Font         font;

    /* ── session ──────────────────────────────────────────── */
    AppScreen scr;
    UserRole  role;
    int       logID;
    int       failCnt;
    bool      locked;

    /* ── message bar ──────────────────────────────────────── */
    char  msgBuf[512];
    bool  msgOn;
    bool  msgErr;

    /* ── scroll ───────────────────────────────────────────── */
    int scrollY;

    /* ── active input-field pointer ───────────────────────── */
    InputField* activeField;

    /* ── login fields ─────────────────────────────────────── */
    InputField fldID;
    InputField fldPwd;

    /* ── general-purpose fields (reused per screen) ───────── */
    InputField flds[6];

    /* ── booking state ────────────────────────────────────── */
    int  bkStep;
    char bkSpec[64];
    int  bkDocID;
    char bkDate[16];
    char bkSlot[8];
    int  bkDateFails;

    /* ── prescribe state ──────────────────────────────────── */
    int  prStep;
    int  prApptID;

    /* ── doctor-history state ─────────────────────────────── */
    bool histSearched;
    int  histPatID;

    /* ── selected items for list screens ─────────────────── */
    int selApptID;
    int selBillID;
    int selDocID;
    int selPatID;

    /* ── security-log cache ───────────────────────────────── */
    char secLines[80][260];
    int  secCount;

    /* ── daily-report cache ───────────────────────────────── */
    bool  reportReady;
    int   rptTotal, rptPend, rptComp, rptNos, rptCanc;
    float rptRev;

    /* ── window constants ─────────────────────────────────── */
    static const int W = 1280;
    static const int H = 800;
    static const int HDR = 78;
    static const int FTR = 52;
    static const int CY = 84;   // content area start Y

    /* ── helpers: string / number ─────────────────────────── */
    void itoStr(int n, char* out) const;
    void ftoStr(float f, char* out, int dec = 2) const;
    void strApp(char* dst, const char* src) const;  // append src→dst

    /* ── helpers: date ────────────────────────────────────── */
    void getTodayStr(char* out)          const;
    int  dateKey(const char* d)          const;   // YYYYMMDD int
    int  cmpDate(const char* a, const char* b);
    bool olderThan(const char* d, int days);

    /* ── helpers: storage queries ─────────────────────────── */
    int  nextApptID();
    int  nextBillID();
    int  nextPresID();
    int  nextDocID();
    bool slotTaken(int docID, const char* date, const char* slot);

    /* ── helpers: sort (fills index array) ────────────────── */
    void sortApptsByDate(int* idx, int n, bool asc);
    void sortApptsByTime(int* idx, int n);
    void sortPresByDate(int* idx, int n, bool asc);

    /* ── UI primitives ────────────────────────────────────── */
    void drawBg();
    void drawHdr(const char* title, const char* sub = nullptr);
    void drawFtr();
    void drawMsg();
    void setMsg(const char* m, bool err = true);
    void clrMsg();

    sf::Text mkTxt(const char* s, float x, float y,
        int sz, sf::Color c = sf::Color::White) const;
    void drawRect(float x, float y, float w, float h,
        sf::Color fill,
        sf::Color outline = sf::Color::Transparent,
        float ot = 0.f);
    bool drawBtn(const char* lbl, float x, float y, float w, float h,
        sf::Color fill, sf::Vector2f mouse,
        sf::Color tc = sf::Color::White);
    bool btnHit(float x, float y, float w, float h, sf::Vector2f p) const;
    void drawRow(float x, float y, float rh,
        const char** cells, const float* widths, int nc,
        sf::Color bg,
        sf::Color tc = sf::Color(215, 230, 255));
    void drawLbl(const char* text, float x, float y,
        sf::Color c = sf::Color(150, 180, 225));

    /* ── per-screen draw ──────────────────────────────────── */
    void drawRole(sf::Vector2f m);
    void drawLogin(sf::Vector2f m);
    void drawPatMenu(sf::Vector2f m);
    void drawPatBook(sf::Vector2f m);
    void drawPatCancel(sf::Vector2f m);
    void drawPatAppts(sf::Vector2f m);
    void drawPatRecords(sf::Vector2f m);
    void drawPatBills(sf::Vector2f m);
    void drawPatPay(sf::Vector2f m);
    void drawPatTopup(sf::Vector2f m);
    void drawDocMenu(sf::Vector2f m);
    void drawDocToday(sf::Vector2f m);
    void drawDocComplete(sf::Vector2f m);
    void drawDocNoshow(sf::Vector2f m);
    void drawDocPrescribe(sf::Vector2f m);
    void drawDocHistory(sf::Vector2f m);
    void drawAdmMenu(sf::Vector2f m);
    void drawAdmAddDoc(sf::Vector2f m);
    void drawAdmRemDoc(sf::Vector2f m);
    void drawAdmPatients(sf::Vector2f m);
    void drawAdmDoctors(sf::Vector2f m);
    void drawAdmAppts(sf::Vector2f m);
    void drawAdmUnpaid(sf::Vector2f m);
    void drawAdmDischarge(sf::Vector2f m);
    void drawAdmSeclog(sf::Vector2f m);
    void drawAdmReport(sf::Vector2f m);

    /* ── click handlers ───────────────────────────────────── */
    void onClick(sf::Vector2f p);
    void onRoleClick(sf::Vector2f p);
    void onLoginClick(sf::Vector2f p);
    void onPatMenuClick(sf::Vector2f p);
    void onPatBookClick(sf::Vector2f p);
    void onPatCancelClick(sf::Vector2f p);
    void onPatPayClick(sf::Vector2f p);
    void onPatTopupClick(sf::Vector2f p);
    void onDocMenuClick(sf::Vector2f p);
    void onDocCompleteClick(sf::Vector2f p);
    void onDocNoshowClick(sf::Vector2f p);
    void onDocPrescribeClick(sf::Vector2f p);
    void onDocHistoryClick(sf::Vector2f p);
    void onAdmMenuClick(sf::Vector2f p);
    void onAdmAddDocClick(sf::Vector2f p);
    void onAdmRemDocClick(sf::Vector2f p);
    void onAdmDischargeClick(sf::Vector2f p);

    /* ── business logic ───────────────────────────────────── */
    bool tryLogin(int id, const char* pwd);
    void doBookAppt();
    void doCancelAppt(int apptID);
    void doPayBill(int billID);
    void doTopup(float amt);
    void doMarkComplete(int apptID);
    void doMarkNoshow(int apptID);
    void doWritePrescription();
    void doAddDoctor();
    void doRemoveDoctor(int docID);
    void doDischarge(int patID);

    /* ── navigation ───────────────────────────────────────── */
    void goTo(AppScreen s);
    void back();
    void logout();

    /* ── init helpers ─────────────────────────────────────── */
    void loadFont();
    void loadData();
    void loadSecLog();
    void computeReport();

public:
    HospitalManagementSystem();
    ~HospitalManagementSystem();
    void run();
};

#endif // HMS_HPP