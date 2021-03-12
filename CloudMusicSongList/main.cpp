#include "main.h"

using namespace std;

static int uid_list(void* notUsed, int argc, char** argv, char** azColName) {
	if (argv[0] == nullptr) {
		return 0;
	}
	uidList.emplace_back(argv[0]);
    stringstream ss(argv[1]);
	string sub;
	while (getline(ss, sub, ',')) {
		if (sub.empty()) {
		    continue;
		}
		uidListInfo[argv[0]].push_back(sub);
	}
	return 0;
}

static int pid_list(void* cnt, int argc, char** argv, char** azColName) {
	long* c = (long*)cnt;
	*c = 0;
	if (argv[0] == nullptr) {
		return 0;
	}
	char* endPtr;
	*c = strtol(argv[0], &endPtr, 10);
	return 0;
}

static int tid_list(void* notUsed, int argc, char** argv, char** azColName) {
	if (argv[0] == nullptr) {
		return 0;
	}
	tidList.emplace_back(argv[0]);
	return 0;
}

static int track_info(void* of, int argc, char** argv, char** azColName) {
	ofstream *f = (ofstream*)of;
    if (argv[0] == nullptr) {
		return 0;
	}
	jsonxx::json j = jsonxx::json::parse(argv[0]);
	cout << j["name"].as_string() << "*****" << j["artists"][0]["name"].as_string() << endl;
	*f << j["name"].as_string() << "*****" << j["artists"][0]["name"].as_string() << endl;
	return 0;
}

bool get_pid(string &pid) {
	char* cErrMsg;
	long cnt = 0;
	string sql = "select count(*) from web_playlist_track where pid=" + pid + ";";
	int ret = sqlite3_exec(db, sql.c_str(), pid_list, &cnt, &cErrMsg);

	if (ret != SQLITE_OK) {
		cout << "get_pid fail: " << cErrMsg << endl;
		return false;
	}
	//cout << "cnt=" << cnt << endl;
	return cnt > 0;
}

void get_track_info(string tid, ofstream &of) {
	char* cErrMsg;
	string sql = "select track from web_track where tid=" + tid + ";";
	int ret = sqlite3_exec(db, sql.c_str(), track_info, &of, &cErrMsg);

	if (ret != SQLITE_OK) {
		cout << "get_trace_info fail: " << cErrMsg << endl;
	}
}

bool get_tid_list(int pid) {
	char* cErrMsg;
	string sql = "select tid from web_playlist_track where pid=" + pidList[pid] + ";";
	int ret = sqlite3_exec(db, sql.c_str(), tid_list, nullptr, &cErrMsg);

	if (ret != SQLITE_OK) {
		cout << "get_tid_list fail: " << cErrMsg << endl;
		return false;
	}

	cout << tidList.size() << " tids exist." << endl;
	return true;
}

void get_pid_list(int uid) {
	for (auto &pid : uidListInfo[uidList[uid]]) {
	    if (get_pid(pid)) {
			pidList.push_back(pid);
	    }
	}
	cout << pidList.size() << " pids exist." << endl;
}

bool get_uid_list() {
	char* cErrMsg;
	int ret = sqlite3_exec(db, "select * from web_user_playlist;", uid_list, nullptr, &cErrMsg);

	if (ret != SQLITE_OK) {
		cout << "get_uid_list fail: " << cErrMsg << endl;
		return false;
	}

	cout << uidList.size() << " uids exist." << endl;
	return true;
}

int main() {
	cout << "Enter webdb.dat path:" << endl;
	string db_path;
	cin >> db_path;
	//打开数据库
    int ret = sqlite3_open(db_path.c_str(), &db);
	if (ret != SQLITE_OK) {
		cout << "Open database fail: " << sqlite3_errmsg(db) << endl;
		return 0;
	}
	//解析uid
	if (!get_uid_list() || uidList.empty()) {
		return 0;
	}
	for (int i = 0; i < uidList.size(); ++i) {
		cout << i + 1 << " => " << uidList[i] << endl;
	}
	cout << "0 => exit" << endl;
	int uid = 0;
	//选择uid
	cout << "Enter the uid's mark number:" << endl;
	cin >> uid;
	if (uid > uidList.size()) {
		cout << "invalid uid's mark number!" << endl;
		return 0;
	}
	if (uid == 0) {
		return 0;
	}
	//解析pid
	get_pid_list(uid - 1);
	if (pidList.empty()) {
		return 0;
	}
	for (int i = 0; i < pidList.size(); ++i) {
		cout << i + 1 << " => " << pidList[i] << endl;
	}
	cout << "0 => exit" << endl;
	//选择pid
	int pid = 0;
	cout << "Enter the pid's mark number:" << endl;
	cin >> pid;
	if (pid > pidList.size()) {
		cout << "invalid pid's mark number!" << endl;
		return 0;
	}
	if (pid == 0) {
		return 0;
	}
	//解析tid
    if (!get_tid_list(pid - 1) || tidList.empty()) {
		return 0;
    }
	//生成存储文件夹
	string path = uidList[uid - 1] + "//";
	if (_access(path.c_str(), 0) != 0) {
		_mkdir(path.c_str());
	}
	path += "//" + pidList[pid - 1] + ".txt";
	ofstream of(path.c_str());
	//解析歌单
    for (auto& tid : tidList) {
		get_track_info(tid, of);
    }
	of.close();
    //关闭数据库
	sqlite3_close(db);
	return 0;
}