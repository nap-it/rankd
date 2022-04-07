use std::path::PathBuf;
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(name = "rankd")]
pub struct RankdOpts {
    #[structopt(
        parse(from_os_str),
        short = "c",
        long = "config",
        value_name = "filename",
        help = "configuration file (default is /etc/rank/config.toml)"
    )]
    pub config_file: Option<PathBuf>,
    #[structopt(
        parse(from_os_str),
        short = "l",
        long = "log-file",
        value_name = "logfile",
        help = "log file location (default is /var/logs/rankd.log)"
    )]
    pub log_file: Option<PathBuf>,
    #[structopt(
        parse(from_occurrences),
        short, long,
        help = "verbosity level (v - info, vv - debug, vvv - trace)"
    )]
    pub verbose: u8,
    #[structopt(
        short = "q",
        long = "quiet",
        value_name = "quiet",
        help = "do not output any message to TTY"
    )]
    pub quiet: bool,
}
