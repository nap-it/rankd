mod capabilities;
mod config;
mod node;
mod options;
mod service;

use failure::Error;
use slog::{info, o, Drain, Duplicate};
use std::fs::OpenOptions;
use structopt::StructOpt;
use tokio::net::TcpListener;
use std::net::Ipv4Addr;

#[tokio::main]
async fn main() {
    // Read options from CLI.
    let cli_options = options::RankdOpts::from_args();

    // Assign default values to CLI options.
    let mut log_file: String = String::from("/var/log/rankd.log");
    let mut quiet: bool = false;
    let mut server_ip: Ipv4Addr = Ipv4Addr::new(127, 0, 0, 1);
    let mut running_deployment_name: String = String::from("Default configuration");

    match &cli_options.config_file {
        Some(file) => {}
        None => {}
    }

    let config = config::parse_file(&cli_options.config_file);

    // Initialize logger.
    let log_file = OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open("rankd.log")
        .unwrap();
    let terminal_decorator = slog_term::TermDecorator::new().build();
    let terminal_drain = slog_term::FullFormat::new(terminal_decorator)
        .build()
        .fuse();
    let terminal_drain = slog_async::Async::new(terminal_drain).build().fuse();
    let file_decorator = slog_term::PlainDecorator::new(log_file);
    let file_drain = slog_term::FullFormat::new(file_decorator).build().fuse();
    let file_drain = slog_async::Async::new(file_drain).build().fuse();
    let log = if !cli_options.quiet {
        slog::Logger::root(Duplicate::new(terminal_drain, file_drain).fuse(), o!())
    } else {
        slog::Logger::root(file_drain, o!())
    };

    info!(log, "rankd is now starting its server.");

    service::start(log).await;
}
