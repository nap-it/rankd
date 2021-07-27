use std::path::{PathBuf, Path};
use openssl::rsa::Rsa;
use crate::errors::SecurityError;
use std::fs::{OpenOptions, File};
use std::io::{Write, Error, Read};

pub fn create_identifier(path: &String, force_creation: bool) -> Result<bool, SecurityError> {
    // Verify if the key is already defined, only if force_creation flag is set.
    if !Path::new(&(path.as_str().to_owned() + "id_rank")).exists() || force_creation {
        let rsa = Rsa::generate(2048).unwrap();
        let private_key = rsa.private_key_to_pem().unwrap();
        let public_key = rsa.public_key_to_pem().unwrap();

        let mut private_key_file = match OpenOptions::new().create(true).write(true).truncate(true).open(path.to_owned() + "id_rank") {
            Ok(file) => file,
            Err(error) => return Err(SecurityError::PermissionDeniedOnKeyPath),
        };
        let mut public_key_file = match OpenOptions::new().create(true).write(true).truncate(true).open(path.to_owned() + "id_rank.pub") {
            Ok(file) => file,
            Err(error) => return Err(SecurityError::PermissionDeniedOnKeyPath),
        };

        private_key_file.write_all(String::from_utf8(private_key).unwrap().as_bytes());
        public_key_file.write_all(String::from_utf8(public_key).unwrap().as_bytes());

        return Ok(true);
    }

    Ok(false)
}

pub fn get_own_identifier(path: String) -> Result<String, SecurityError> {
    let mut public_key_file = OpenOptions::new().read(true).open(path + "id_rank.pub").unwrap();
    let mut public_key_contents = String::new();
    public_key_file.read_to_string(&mut public_key_contents).unwrap();
    let public_key = Rsa::public_key_from_pem(public_key_contents.as_bytes()).unwrap();
    let identifier = base64::encode(public_key.public_key_to_der().unwrap());

    Ok(identifier)
}

// #[cfg(test)]
// mod tests {
//     use crate::security::create_hash;
//     use std::path::PathBuf;
//
//     #[test]
//     fn create_hash_test() {
//         let hash = create_hash(PathBuf::from("/Users/ruilopes/tests/"), true).unwrap();
//         println!("private: \n{}", String::from_utf8(hash.0).unwrap());
//         println!("public: \n{}", String::from_utf8(hash.1).unwrap());
//     }
// }