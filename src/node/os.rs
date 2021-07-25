use std::fmt::{Display, Formatter};
use sysinfo::{Gid, Pid, ProcessExt, ProcessStatus, SystemExt, Uid, UserExt};
use serde::{Serialize, Deserialize};

#[derive(Debug)]
pub struct User {
    uid: Uid,
    gid: Gid,
    name: String,
    groups: Vec<String>,
}

impl User {
    pub fn get(user: &sysinfo::User) -> Self {
        User {
            uid: user.uid(),
            gid: user.gid(),
            name: user.name().to_string(),
            groups: user.groups().to_vec(),
        }
    }

    pub fn get_all(system: &sysinfo::System) -> Vec<Self> {
        let mut users: Vec<User> = Vec::new();

        for user in system.users() {
            users.push(User::get(user));
        }

        users
    }
}

impl Display for User {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "USR -> [name: {}, uid: {:?}, gid: {:?}]",
            self.name, self.uid, self.gid
        )
    }
}

#[derive(Debug)]
pub struct Process {
    name: String,
    command: String,
    path: String,
    pid: Pid,
    memory_used: u64,
    virtual_memory: u64,
    parent: Option<Pid>,
    status: ProcessStatus,
    start_time: u64,
    cpu_usage: f32,
    disk_read_bytes: u64,
    disk_written_bytes: u64,
}

impl Process {
    pub fn parse(process: &sysinfo::Process) -> Self {
        Process {
            name: process.name().to_owned(),
            command: {
                let mut command = String::new();
                for directory in process.cmd().to_vec() {
                    command += String::from(format!("{}/", directory)).as_str();
                }
                command.pop();
                command
            },
            path: String::from(process.exe().to_owned().to_str().unwrap()),
            pid: process.pid(),
            memory_used: process.memory(),
            virtual_memory: process.virtual_memory(),
            parent: process.parent(),
            status: process.status(),
            start_time: process.start_time(),
            cpu_usage: process.cpu_usage(),
            disk_read_bytes: process.disk_usage().total_read_bytes,
            disk_written_bytes: process.disk_usage().total_written_bytes,
        }
    }

    pub fn update_all(system: &sysinfo::System) -> Vec<Process> {
        let mut processes: Vec<Process> = Vec::new();

        for (_, process) in system.processes() {
            processes.push(Process::parse(process));
        }

        processes
    }
}

impl Display for Process {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(
            f,
            "PRC -> [name: {}, cmd: {}, pid: {}, mem-used: {}, virt-mem: {}, cpu: {}, disk: {}/{}]",
            self.name,
            self.command,
            self.pid,
            self.memory_used,
            self.virtual_memory,
            self.cpu_usage,
            self.disk_read_bytes,
            self.disk_written_bytes
        )
    }
}

#[derive(Debug)]
pub struct OperativeSystem {
    users: Vec<User>,
    processes: Vec<Process>,
}

impl OperativeSystem {
    pub fn update(system: &sysinfo::System) -> Self {
        OperativeSystem {
            users: User::get_all(system),
            processes: Process::update_all(system),
        }
    }
}

impl Display for OperativeSystem {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "OS  -> [users active: {}, processes: {}]", self.users.len(), self.processes.len())
    }
}
