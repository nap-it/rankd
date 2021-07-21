use std::path::PathBuf;
use structopt::StructOpt;

#[derive(Debug, StructOpt)]
#[structopt(name = "rankd")]
pub struct RankdOpts {
    //#[structopt(
    //    parse(from_os_str),
    //    short = "c",
    //    long = "config",
    //    value_name = "filename",
    //    help = "configuration file"
    //)]
    //pub config_file: PathBuf,
    #[structopt(
        short = "q",
        long = "quiet",
        value_name = "quiet",
        help = "do not output any message"
    )]
    pub quiet: bool,
}
