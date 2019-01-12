// Copyright 2018 (c) rust-themis developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern crate themis;

use themis::{secure_comparator::SecureComparator, ErrorKind};

#[test]
fn compare_matching_data() {
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    comparator1.append_secret(b"se-e-ecrets").unwrap();
    comparator2.append_secret(b"se-e-ecrets").unwrap();

    let data = comparator1.begin_compare().unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    assert!(comparator1.is_complete());
    assert!(comparator2.is_complete());

    assert!(comparator1.get_result().unwrap());
    assert!(comparator2.get_result().unwrap());
}

#[test]
fn compare_different_data() {
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    assert!(!comparator1.is_complete());
    assert!(!comparator2.is_complete());

    comparator1
        .append_secret(b"far from the worn path of reason")
        .unwrap();
    comparator2
        .append_secret(b"further away from the sane")
        .unwrap();

    let data = comparator1.begin_compare().unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    assert!(!comparator1.get_result().unwrap());
    assert!(!comparator2.get_result().unwrap());
}

#[test]
fn split_secrets() {
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    comparator1.append_secret(b"123").unwrap();
    comparator1.append_secret(b"456").unwrap();
    comparator2.append_secret(b"123456").unwrap();

    let data = comparator1.begin_compare().unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    assert!(comparator1.get_result().unwrap());
    assert!(comparator2.get_result().unwrap());
}

#[test]
fn simultaneous_start() {
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    comparator1.append_secret(b"se-e-ecrets").unwrap();
    comparator2.append_secret(b"se-e-ecrets").unwrap();

    let data1 = comparator1.begin_compare().unwrap();
    let data2 = comparator2.begin_compare().unwrap();

    let error1 = comparator1.proceed_compare(&data2).unwrap_err();
    let error2 = comparator2.proceed_compare(&data1).unwrap_err();

    assert_eq!(error1.kind(), ErrorKind::InvalidParameter);
    assert_eq!(error2.kind(), ErrorKind::InvalidParameter);

    assert!(!comparator1.is_complete());
    assert!(!comparator2.is_complete());
}

// TODO: write some robust test for data corruption
//
// This one works, but the results are intermittent. Sometimes the comparisons don't match,
// sometimes the comparators fail with 'invalid parameter' errors. Maybe we could make use
// of some data fuzzing framework in the future.
#[test]
#[ignore]
fn data_corruption() {
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    comparator1.append_secret(b"se-e-ecrets").unwrap();
    comparator2.append_secret(b"se-e-ecrets").unwrap();

    let data = comparator1.begin_compare().unwrap();
    let mut data = comparator2.proceed_compare(&data).unwrap();
    data[20] = 42;
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    assert!(comparator1.get_result().unwrap());
    assert!(comparator2.get_result().unwrap());
}

// TODO: investigate crash in BoringSSL
//
// This tests crashes when Themis uses BoringSSL. It suggests that one cannot reuse Secure
// Comparators at all because the crypto engine denies this (e.g., can't compute hash twice).
// Investigate, probably file an issue in core Themis repo as this state should be tracked
// by Themis library to avoid misuse. However, this may be a feature of Secure Comparator
// so it may require a workaround.
#[test]
fn reusing_comparators() {
    // TODO: avoid reusing comparators via a better API
    let mut comparator1 = SecureComparator::new();
    let mut comparator2 = SecureComparator::new();

    comparator1.append_secret(b"test").unwrap();
    comparator2.append_secret(b"data").unwrap();

    let data = comparator1.begin_compare().unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    assert!(!comparator1.get_result().unwrap());
    assert!(!comparator2.get_result().unwrap());

    comparator1.append_secret(b"same").unwrap();
    comparator2.append_secret(b"same").unwrap();

    let data = comparator1.begin_compare().unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let data = comparator1.proceed_compare(&data).unwrap();
    let data = comparator2.proceed_compare(&data).unwrap();
    let _ata = comparator1.proceed_compare(&data).unwrap();

    // Previous data is still appended and can't be unappended.
    assert!(!comparator1.get_result().unwrap());
    assert!(!comparator2.get_result().unwrap());
}
