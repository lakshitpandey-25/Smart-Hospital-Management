"use strict";

const CAPACITY = 20;
const patients = [];
let nextOrder = 0;

const form = document.getElementById("patientForm");
const table = document.getElementById("patientTable");
const search = document.getElementById("searchPatient");
const message = document.getElementById("formMessage");

const totalPatientsElement = document.getElementById("totalPatients");
const waitingPatientsElement = document.getElementById("waitingPatients");
const emergencyPatientsElement = document.getElementById("emergencyPatients");
const availableBedsElement = document.getElementById("availableBeds");

const freeBedsElement = document.getElementById("freeBeds");
const occupiedBedsElement = document.getElementById("occupiedBeds");

document.getElementById("currentDate").textContent =
    new Date().toLocaleDateString("en-IN", {
        day: "numeric",
        month: "short",
        year: "numeric"
    });

function showMessage(text, isError = false) {
    message.textContent = text;
    message.className = isError ? "error" : "success";
}

function addCell(row, value) {
    const cell = document.createElement("td");
    cell.textContent = value;
    row.appendChild(cell);
    return cell;
}

function renderQueue(elementId, items, emptyText) {
    const list = document.getElementById(elementId);
    list.replaceChildren();

    if (items.length === 0) {
        const item = document.createElement("li");
        item.textContent = emptyText;
        item.className = "empty-queue";
        list.appendChild(item);
        return;
    }

    items.forEach(patient => {
        const item = document.createElement("li");

        item.textContent =
            `${patient.id} — ${patient.name} · Level ${patient.severity}`;

        list.appendChild(item);
    });
}

function renderTable() {
    const query = search.value.trim().toLowerCase();

    const filteredPatients = patients.filter(patient =>
        patient.id.toLowerCase().includes(query)
    );

    table.replaceChildren();

    if (filteredPatients.length === 0) {
        const row = document.createElement("tr");
        const cell = addCell(
            row,
            patients.length > 0
                ? "No matching patients found."
                : "No patients registered yet."
        );

        cell.colSpan = 6;
        cell.className = "empty-row";
        table.appendChild(row);
        return;
    }

    filteredPatients.forEach(patient => {
        const row = document.createElement("tr");

        addCell(row, patient.id);
        addCell(row, patient.name);
        addCell(row, String(patient.age));
        addCell(row, `Level ${patient.severity}`);
        addCell(row, patient.department);

        const statusCell = addCell(row, "");
        const badge = document.createElement("span");

        badge.className =
            patient.severity <= 2
                ? "pill emergency"
                : "pill";

        badge.textContent =
            patient.severity <= 2
                ? "Priority Queue"
                : "Waiting";

        statusCell.appendChild(badge);
        table.appendChild(row);
    });
}

function renderResources() {
    // This is a demo: registering a patient does not assign a bed.
    const occupiedBeds = 0;
    const availableBeds = CAPACITY - occupiedBeds;

    availableBedsElement.textContent = availableBeds;
    freeBedsElement.textContent = availableBeds;
    occupiedBedsElement.textContent = occupiedBeds;

    document.getElementById("availableProgress").style.width =
        `${(availableBeds / CAPACITY) * 100}%`;

    document.getElementById("occupiedProgress").style.width =
        `${(occupiedBeds / CAPACITY) * 100}%`;
}

function renderDashboard() {
    const emergencyPatients = patients
        .filter(patient => patient.severity <= 2)
        .sort((a, b) =>
            a.severity - b.severity || a.order - b.order
        );

    const normalPatients = patients.filter(
        patient => patient.severity >= 3
    );

    totalPatientsElement.textContent = patients.length;

    // Normal waiting queue only; priority patients are counted separately.
    waitingPatientsElement.textContent = normalPatients.length;

    emergencyPatientsElement.textContent = emergencyPatients.length;

    renderTable();

    renderQueue(
        "normalQueue",
        normalPatients,
        "No patients waiting."
    );

    renderQueue(
        "emergencyQueue",
        emergencyPatients,
        "No priority patients."
    );

    renderResources();
}

form.addEventListener("submit", function(event) {
    event.preventDefault();

    const id = document.getElementById("patientId").value.trim();
    const name = document.getElementById("patientName").value.trim();
    const ageValue = document.getElementById("patientAge").value;
    const severityValue = document.getElementById("severity").value;
    const department = document.getElementById("department").value;

    const age = Number(ageValue);
    const severity = Number(severityValue);

    if (
        !id ||
        !name ||
        !department ||
        ageValue === "" ||
        !Number.isInteger(age) ||
        age < 0 ||
        age > 120 ||
        ![1, 2, 3, 4, 5].includes(severity)
    ) {
        showMessage("Please enter valid details in every field.", true);
        return;
    }

    const duplicate = patients.some(patient =>
        patient.id.toLowerCase() === id.toLowerCase()
    );

    if (duplicate) {
        showMessage(
            "This Patient ID already exists. Please use another ID.",
            true
        );
        return;
    }

    patients.push({
        id,
        name,
        age,
        severity,
        department,
        order: nextOrder++
    });

    form.reset();
    search.value = "";

    renderDashboard();
    showMessage("Patient registered successfully.");
});

search.addEventListener("input", renderTable);

renderDashboard();