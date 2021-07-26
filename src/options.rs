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
        help = "configuration file"
    )]
    pub config_file: Option<PathBuf>,
    #[structopt(
        short = "l",
        long = "log-file",
        value_name = "logfile",
        help = "log file location (default is /var/logs/rankd.log)"
    )]
    pub log_file: Option<PathBuf>,
    #[structopt(
        short = "v",
        long = "verbose",
        value_name = "verbose",
        help = "verbosity level (v - info, vv - debug, vvv - trace)"
    )]
    pub verbose: bool,
    #[structopt(
        short = "q",
        long = "quiet",
        value_name = "quiet",
        help = "do not output any message"
    )]
    pub quiet: bool,
}
