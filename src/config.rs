use failure::Error;
use serde::Deserialize;
use std::fs;
use std::path::PathBuf;
use toml;

pub fn parse_file(config_file: &PathBuf) -> Result<RankdConfig, Error> {
    let config = fs::read_to_string(config_file)?;
    Ok(toml::from_str(&config)?)
}

#[derive(Clone, Debug, Deserialize)]
pub struct RankdConfig {
    pub title: String,
}
