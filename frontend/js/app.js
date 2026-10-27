"use strict";

const patients = [];
let normalQueue = [];
let emergencyQueue = [];
let resources = { total: 20, available: 20, occupied: 0 };
let displayedPatients = [];

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

    if (!items.length) {
        const item = document.createElement("li");
        item.textContent = emptyText;
        item.className = "empty-queue";
        list.appendChild(item);
        return;
    }

    items.forEach(patient => {
        const item = document.createElement("li");
        item.textContent = `${patient.id} — ${patient.name} · Level ${patient.severity}`;
        list.appendChild(item);
    });
}

function renderTable(items = displayedPatients) {
    table.replaceChildren();

    if (!items.length) {
        const row = document.createElement("tr");
        const cell = addCell(
            row,
            search.value.trim()
                ? "No matching patients found."
                : "No patients registered yet."
        );
        cell.colSpan = 6;
        cell.className = "empty-row";
        table.appendChild(row);
        return;
    }

    items.forEach(patient => {
        const row = document.createElement("tr");
        addCell(row, String(patient.id));
        addCell(row, patient.name);
        addCell(row, String(patient.age));
        addCell(row, `Level ${patient.severity}`);
        addCell(row, patient.department);

        const statusCell = addCell(row, "");
        const badge = document.createElement("span");
        badge.className = patient.severity <= 2 ? "pill emergency" : "pill";
        badge.textContent = patient.severity <= 2 ? "Priority Queue" : "Waiting";
        statusCell.appendChild(badge);
        statusCell.appendChild(document.createElement("br"));

        const bedButton = document.createElement("button");
        bedButton.type = "button";
        bedButton.textContent = patient.hasBed ? "Release bed" : "Assign bed";
        bedButton.className = "bed-action-button";
        bedButton.addEventListener("click", () => changeBed(patient));
        statusCell.appendChild(bedButton);
table.appendChild(row);
    });
}

function renderResources() {
    const total = resources.total || 20;
    const available = resources.available || 0;
    const occupied = resources.occupied || 0;

    availableBedsElement.textContent = available;
    freeBedsElement.textContent = available;
    occupiedBedsElement.textContent = occupied;

    document.getElementById("availableProgress").style.width =
        `${Math.max(0, Math.min(100, (available / total) * 100))}%`;
    document.getElementById("occupiedProgress").style.width =
        `${Math.max(0, Math.min(100, (occupied / total) * 100))}%`;
}

function renderDashboard() {
    totalPatientsElement.textContent = patients.length;
    waitingPatientsElement.textContent = normalQueue.length;
    emergencyPatientsElement.textContent = emergencyQueue.length;

    renderTable(displayedPatients);
    renderQueue("normalQueue", normalQueue, "No patients waiting.");
    renderQueue("emergencyQueue", emergencyQueue, "No priority patients.");
    renderResources();
}

async function loadDashboard() {
    try {
        const response = await fetch("/api/patients");
        const data = await response.json();
        if (!response.ok || !data.ok) throw new Error(data.error || "Could not load patients.");

        patients.splice(0, patients.length, ...data.patients);
        normalQueue = data.normalQueue;
        emergencyQueue = data.emergencyQueue;
        resources = data.resources;

        const query = search.value.trim();
        if (query) {
            await runSearch();
        } else {
            displayedPatients = [...patients];
            renderDashboard();
        }
    } catch (error) {
        showMessage(`Backend connection error: ${error.message}`, true);
    }
}

async function runSearch() {
    const query = search.value.trim();
    if (!query) {
        displayedPatients = [...patients];
        renderTable(displayedPatients);
        return;
    }

    try {
        const response = await fetch(`/api/patients?search=${encodeURIComponent(query)}`);
        const data = await response.json();
        displayedPatients = response.ok && data.ok ? [data.patient] : [];
        renderTable(displayedPatients);
    } catch (error) {
        showMessage("Could not search patients. Check the backend server.", true);
    }
}

async function changeBed(patient) {
    const action = patient.hasBed ? "release" : "assign";
    try {
        const response = await fetch("/api/beds", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ action, patient_id: patient.id })
        });
        const data = await response.json();
        if (!response.ok || !data.ok) throw new Error(data.error || "Bed operation failed.");
        await loadDashboard();
        showMessage(data.message);
    } catch (error) {
        showMessage(error.message, true);
    }
}

form.addEventListener("submit", async function(event) {
    event.preventDefault();

    const id = document.getElementById("patientId").value.trim();
    const name = document.getElementById("patientName").value.trim();
    const ageValue = document.getElementById("patientAge").value;
    const severityValue = document.getElementById("severity").value;
    const department = document.getElementById("department").value;

    const numericId = Number(id);
    const age = Number(ageValue);
    const severity = Number(severityValue);

    if (
        !id || !Number.isInteger(numericId) || numericId <= 0 ||
        !name || !department || ageValue === "" ||
        !Number.isInteger(age) || age < 0 || age > 120 ||
        ![1, 2, 3, 4, 5].includes(severity)
    ) {
        showMessage("Enter a positive numeric ID and valid details in every field.", true);
        return;
    }

    try {
        const response = await fetch("/api/patients", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ id: numericId, name, age, severity, department })
        });
        const data = await response.json();
        if (!response.ok || !data.ok) throw new Error(data.error || "Registration failed.");

        form.reset();
        search.value = "";
        await loadDashboard();
        showMessage(data.message || "Patient registered successfully.");
    } catch (error) {
        showMessage(error.message, true);
    }
});

search.addEventListener("input", runSearch);
loadDashboard();
